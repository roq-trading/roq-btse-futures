/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/utils/hash/fnv.hpp"

#include "roq/core/json/parser.hpp"

#include "roq/btse_futures/json/event.hpp"
#include "roq/btse_futures/json/topic.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace json {

// === CONSTANTS ===

namespace {
auto const PONG = "pong"sv;

constexpr auto const KEY_EVENT = "event"sv;
constexpr auto const KEY_TOPIC = "topic"sv;
}  // namespace

// === HELPERS ===

namespace {
template <typename T>
auto dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj);
  return true;
}

constexpr auto extract_topic_helper(std::string_view const &topic) {
  auto pos = topic.find_first_of(':');
  return topic.substr(0, pos);
}

static_assert(extract_topic_helper(""sv) == ""sv);
static_assert(extract_topic_helper("tradeHistoryApiV3"sv) == "tradeHistoryApiV3"sv);
static_assert(extract_topic_helper("tradeHistoryApiV3:BTC-PERP"sv) == "tradeHistoryApiV3"sv);

auto extract_topic(std::string_view const &topic) {
  return Topic{extract_topic_helper(topic)};
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
  auto result = false;
  auto helper = [&](auto &key, auto &value) {
    auto key_2 = utils::hash::FNV::compute(key);
    switch (key_2) {
      case utils::hash::FNV::compute(KEY_EVENT): {
        Event event{value};
        switch (event) {
          using enum Event::type_t;
          case UNDEFINED_INTERNAL:
            log::fatal("Unexpected"sv);
          case UNKNOWN_INTERNAL:
            return true;
          case LOGIN:
            result = dispatch_helper<Login>(handler, message, buffer_stack, trace_info);
            return true;
          case SUBSCRIBE:
            result = dispatch_helper<Subscribe>(handler, message, buffer_stack, trace_info);
            return true;
        }
        break;
      }
      case utils::hash::FNV::compute(KEY_TOPIC): {
        auto topic = extract_topic(std::get<std::string_view>(value));
        switch (topic) {
          using enum Topic::type_t;
          case UNDEFINED_INTERNAL:
            log::fatal("Unexpected"sv);
          case UNKNOWN_INTERNAL:
            return true;
          case TRADE_HISTORY_API:
            result = dispatch_helper<TradeHistory>(handler, message, buffer_stack, trace_info);
            return true;
          case SNAPSHOT_L1:
            result = dispatch_helper<SnapshotL1>(handler, message, buffer_stack, trace_info);
            return true;
          case UPDATE:
            result = dispatch_helper<Update>(handler, message, buffer_stack, trace_info);
            return true;
            //
          case POSITIONS_V3:
            result = dispatch_helper<Positions>(handler, message, buffer_stack, trace_info);
            return true;
          case ALL_POSITION_V4:
            result = dispatch_helper<AllPosition>(handler, message, buffer_stack, trace_info);
            return true;
          case NOTIFICATION_API_V4:
            result = dispatch_helper<Notification>(handler, message, buffer_stack, trace_info);
            return true;
          case FILLS_V2:
            result = dispatch_helper<Fills>(handler, message, buffer_stack, trace_info);
            return true;
        }
        break;
      }
    }
    return result;
  };
  core::json::Parser::dispatch<core::json::Object>(helper, message);
  if (result || allow_unknown_event_types) {
    return result;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace btse_futures
}  // namespace roq
