/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/btse_futures/gateway/account.hpp"
#include "roq/btse_futures/gateway/shared.hpp"

#include "roq/btse_futures/protocol/json/parser.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

struct DropCopy final : public Base<DropCopy>, public server::Stream, public web::socket::Client::Handler, protocol::json::Parser::Handler {
  struct Handler {};

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &);

 protected:
  friend base_type;

  // server::Stream

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason) override;

  // web::socket::Client::Handler

  void operator()(Trace<web::socket::Connected> const &) override;
  void operator()(Trace<web::socket::Disconnected> const &) override;
  void operator()(Trace<web::socket::Ready> const &) override;
  void operator()(Trace<web::socket::Close> const &) override;
  void operator()(Trace<web::socket::Latency> const &) override;
  void operator()(Trace<web::socket::Text> const &) override;
  void operator()(Trace<web::socket::Binary> const &) override;

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

  // helpers

  void login(TraceInfo const &);

  void subscribe();

  void subscribe(std::string_view const &topic);

  void parse(TraceInfo const &, std::string_view const &message);

 private:
  [[maybe_unused]] Handler &handler_;
  //
  std::string const name_;
  std::unique_ptr<web::socket::Client> const connection_;
  //
  core::json::BufferStack decode_buffer_;
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  Account &account_;
  Shared &shared_;
  std::chrono::nanoseconds logon_timeout_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
