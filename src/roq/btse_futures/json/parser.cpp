/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/btse_futures/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/btse_futures/json/message.hpp"
#include "roq/btse_futures/json/topic.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace json {

// === CONSTANTS ===

namespace {
auto const PONG = "pong"sv;
}

// === HELPERS ===

namespace {
template <typename T>
void dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj);
}

auto extract_topic(std::string_view const &topic) {
  auto pos = topic.find_first_of(':');
  return Topic{topic.substr(0, pos)};
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  if (message == PONG) [[unlikely]] {
    Pong pong;
    create_trace_and_dispatch(handler, trace_info, pong);
    return true;
  }
  Message message_2{message, buffer_stack};
  switch (message_2.event) {
    using enum Event::type_t;
    case UNDEFINED_INTERNAL: {
      auto topic = extract_topic(message_2.topic);
      switch (topic) {
        using enum Topic::type_t;
        case UNDEFINED_INTERNAL:
          assert(false);
        case UNKNOWN_INTERNAL:
          if (allow_unknown_event_types) {
            return false;
          }
          break;
        case TRADE_HISTORY_API:
          dispatch_helper<TradeHistory>(handler, message, buffer_stack, trace_info);
          return true;
        case SNAPSHOT_L1:
          dispatch_helper<SnapshotL1>(handler, message, buffer_stack, trace_info);
          return true;
        case UPDATE:
          dispatch_helper<Update>(handler, message, buffer_stack, trace_info);
          return true;
      }
      break;
    }
    case UNKNOWN_INTERNAL:
      if (allow_unknown_event_types) {
        return false;
      }
      break;
    case SUBSCRIBE:
      // drop
      return true;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace btse_futures
}  // namespace roq
