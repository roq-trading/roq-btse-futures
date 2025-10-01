/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/ack.hpp"
#include "roq/btse_futures/json/pong.hpp"

#include "roq/btse_futures/json/book.hpp"
#include "roq/btse_futures/json/kline.hpp"
#include "roq/btse_futures/json/market24h.hpp"
#include "roq/btse_futures/json/trades.hpp"

#include "roq/btse_futures/json/login.hpp"

#include "roq/btse_futures/json/account.hpp"
#include "roq/btse_futures/json/fill.hpp"
#include "roq/btse_futures/json/order.hpp"
#include "roq/btse_futures/json/position.hpp"

namespace roq {
namespace btse_futures {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Pong> const &) = 0;
    virtual void operator()(Trace<json::Ack> const &) = 0;
    //
    virtual void operator()(Trace<json::Book> const &) = 0;
    virtual void operator()(Trace<json::Trades> const &) = 0;
    virtual void operator()(Trace<json::Market24h> const &) = 0;
    virtual void operator()(Trace<json::Kline> const &) = 0;
    //
    virtual void operator()(Trace<json::Login> const &) = 0;
    virtual void operator()(Trace<json::Account> const &) = 0;
    virtual void operator()(Trace<json::Position> const &) = 0;
    virtual void operator()(Trace<json::Order> const &) = 0;
    virtual void operator()(Trace<json::Fill> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace btse_futures
}  // namespace roq
