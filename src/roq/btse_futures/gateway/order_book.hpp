/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <deque>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/btse_futures/gateway/shared.hpp"

#include "roq/btse_futures/protocol/json/parser.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

struct OrderBook final : public web::socket::Client::Handler, public protocol::json::Parser::Handler {
  struct Handler {};

  OrderBook(Handler &, io::Context &, uint16_t stream_id, Shared &, size_t index);

  OrderBook(OrderBook const &) = delete;

  uint16_t stream_id() const { return stream_id_; }

  bool ready() const { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &) const;

  void subscribe(size_t start_from = 0);

 protected:
  // web::socket::Client::Handler

  void operator()(web::socket::Client::Connected const &) override;
  void operator()(web::socket::Client::Disconnected const &) override;
  void operator()(web::socket::Client::Ready const &) override;
  void operator()(web::socket::Client::Close const &) override;
  void operator()(web::socket::Client::Latency const &) override;
  void operator()(web::socket::Client::Text const &) override;
  void operator()(web::socket::Client::Binary const &) override;

 private:
  void operator()(ConnectionStatus, std::string_view const &reason = {});

  void ping(std::chrono::nanoseconds now);

  void subscribe(std::span<Symbol const> const &symbols);
  void subscribe(std::span<Symbol const> const &symbols, std::string_view const &channel);
  void subscribe(std::span<Symbol const> const &symbols, std::string_view const &channel, uint32_t group);

  void parse(std::string_view const &message);

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
  [[maybe_unused]] Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  size_t const index_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse, snapshot_l1, update;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // cache
  Shared &shared_;
  // state
  ConnectionStatus connection_status_ = {};
  std::chrono::nanoseconds next_ping_ = {};
};

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
