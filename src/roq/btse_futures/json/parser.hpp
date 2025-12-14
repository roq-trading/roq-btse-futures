/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/pong.hpp"
#include "roq/btse_futures/json/subscribe.hpp"

#include "roq/btse_futures/json/trade_history.hpp"

#include "roq/btse_futures/json/snapshot_l1.hpp"
#include "roq/btse_futures/json/update.hpp"

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
    virtual void operator()(Trace<json::Subscribe> const &) = 0;
    //
    virtual void operator()(Trace<json::TradeHistory> const &) = 0;
    //
    virtual void operator()(Trace<json::SnapshotL1> const &) = 0;
    virtual void operator()(Trace<json::Update> const &) = 0;
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
