/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/btse_futures/gateway/account.hpp"
#include "roq/btse_futures/gateway/shared.hpp"

#include "roq/btse_futures/protocol/json/parser.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

struct DropCopy final : public web::socket::Client::Handler, protocol::json::Parser::Handler {
  struct Handler {
    virtual void operator()(Trace<StreamStatus> const &) = 0;
    virtual void operator()(Trace<ExternalLatency> const &) = 0;
    virtual void operator()(Trace<TradeUpdate> const &, bool is_last, uint8_t user_id) = 0;
    virtual void operator()(Trace<PositionUpdate> const &, bool is_last) = 0;
  };

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

  DropCopy(DropCopy const &) = delete;

  bool ready() const;

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &) const;

 protected:
  // web::socket::Client::Handler

  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

  // protocol::json::Parser::Handler

  void operator()(Trace<protocol::json::Pong> const &) override;
  void operator()(Trace<protocol::json::Subscribe> const &) override;
  //
  void operator()(Trace<protocol::json::TradeHistory> const &) override;
  //
  void operator()(Trace<protocol::json::SnapshotL1> const &) override;
  void operator()(Trace<protocol::json::Update> const &) override;
  //
  void operator()(Trace<protocol::json::Login> const &) override;
  void operator()(Trace<protocol::json::Positions> const &) override;
  void operator()(Trace<protocol::json::AllPosition> const &) override;
  void operator()(Trace<protocol::json::Notification> const &) override;
  void operator()(Trace<protocol::json::Fills> const &) override;

 private:
  void operator()(ConnectionStatus, std::string_view const &reason = {});

  void login();

  void subscribe();

  void subscribe(std::string_view const &topic);

  void parse(std::string_view const &message);

 private:
  Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  // state
  bool ready_ = false;
  ConnectionStatus connection_status_ = {};
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
