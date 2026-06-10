/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/protocol/json/pong.hpp"
#include "roq/btse_futures/protocol/json/subscribe.hpp"

#include "roq/btse_futures/protocol/json/trade_history.hpp"

#include "roq/btse_futures/protocol/json/snapshot_l1.hpp"
#include "roq/btse_futures/protocol/json/update.hpp"

#include "roq/btse_futures/protocol/json/login.hpp"

#include "roq/btse_futures/protocol/json/all_position.hpp"
#include "roq/btse_futures/protocol/json/fills.hpp"
#include "roq/btse_futures/protocol/json/notification.hpp"
#include "roq/btse_futures/protocol/json/positions.hpp"

namespace roq {
namespace btse_futures {
namespace protocol {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<protocol::json::Pong> const &) = 0;
    virtual void operator()(Trace<protocol::json::Subscribe> const &) = 0;
    //
    virtual void operator()(Trace<protocol::json::TradeHistory> const &) = 0;
    //
    virtual void operator()(Trace<protocol::json::SnapshotL1> const &) = 0;
    virtual void operator()(Trace<protocol::json::Update> const &) = 0;
    //
    virtual void operator()(Trace<protocol::json::Login> const &) = 0;
    virtual void operator()(Trace<protocol::json::Positions> const &) = 0;
    virtual void operator()(Trace<protocol::json::AllPosition> const &) = 0;
    virtual void operator()(Trace<protocol::json::Notification> const &) = 0;
    virtual void operator()(Trace<protocol::json::Fills> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace protocol
}  // namespace btse_futures
}  // namespace roq
