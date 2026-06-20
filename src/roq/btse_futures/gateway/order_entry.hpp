/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/download.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/btse_futures/gateway/account.hpp"
#include "roq/btse_futures/gateway/shared.hpp"

#include "roq/btse_futures/protocol/json/get_open_orders_ack.hpp"
#include "roq/btse_futures/protocol/json/get_position_mode_ack.hpp"
#include "roq/btse_futures/protocol/json/get_positions_ack.hpp"
#include "roq/btse_futures/protocol/json/get_wallet_ack.hpp"

#include "roq/btse_futures/protocol/json/cancel_all_orders_ack.hpp"
#include "roq/btse_futures/protocol/json/cancel_order_ack.hpp"
#include "roq/btse_futures/protocol/json/modify_order_ack.hpp"
#include "roq/btse_futures/protocol/json/place_order_ack.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

struct OrderEntry final : public web::rest::Client::Handler {
  struct Handler {};

  OrderEntry(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, bool master);

  OrderEntry(OrderEntry const &) = delete;

  bool ready() const { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Event<Start> const &);
  void operator()(Event<Stop> const &);
  void operator()(Event<Timer> const &);

  void operator()(metrics::Writer &) const;

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  uint16_t operator()(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  uint16_t operator()(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id);

 protected:
  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Client::Connected> const &) override;
  void operator()(Trace<web::rest::Client::Disconnected> const &) override;
  void operator()(Trace<web::rest::Client::Latency> const &) override;

  // helpers

  void operator()(ConnectionStatus, std::string_view const &reason = {});

  enum class State {
    UNDEFINED = 0,
    POSITION_MODE,
    WALLET,
    POSITIONS,
    OPEN_ORDERS,
    FILL_HISTORY,
    DONE,
  };

  uint32_t download(State);

  // position-mode

  void get_position_mode();
  void get_position_mode_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::GetPositionModeAck> const &);

  // wallet

  void get_wallet();
  void get_wallet_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::GetWalletAck> const &);

  // positions

  void get_positions();
  void get_positions_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::GetPositionsAck> const &);

  // open_orders

  void get_open_orders();
  void get_open_orders_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::GetOpenOrdersAck> const &);

  /*
  // fill_history

  void get_fill_history();
  void get_fill_history_ack(Trace<web::rest::Response> const &, uint32_t sequence);
  void operator()(Trace<protocol::json::FillHistory> const &);
  */

  // create-order

  void create_order(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  void create_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::PlaceOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // amend-order

  void amend_order(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void amend_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::ModifyOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-order

  void cancel_order(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void cancel_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::CancelOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-all-orders

  void cancel_all_orders(Event<CancelAllOrders> const &, std::string_view const &request_id);
  void cancel_all_orders_ack(Trace<web::rest::Response> const &, std::string_view const &request_id);
  void operator()(Trace<protocol::json::CancelAllOrdersAck> const &, std::string_view const &request_id);

  // cancel-all-after

  void cancel_all_after();
  void cancel_all_after_ack(Trace<web::rest::Response> const &);
  // void operator()(Trace<protocol::json::CancelAllOrdersAck> const &, uint8_t user_id);

  // helpers

  void process_response(web::rest::Response const &, auto error_handler, auto success_handler);

 private:
  Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  bool const master_;
  // connection
  std::unique_ptr<web::rest::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile  //
        position_mode,
        position_mode_ack,                         //
        wallet, wallet_ack,                        //
        positions, positions_ack,                  //
        open_orders, open_orders_ack,              //
        fill_history, fill_history_ack,            //
        place_order, place_order_ack,              //
        modify_order, modify_order_ack,            //
        cancel_order, cancel_order_ack,            //
        cancel_all_orders, cancel_all_orders_ack,  //
        countdown_cancel_all, countdown_cancel_all_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // account
  Account &account_;
  Shared &shared_;
  // state
  ConnectionStatus connection_status_ = {};
  core::Download<State> download_;
  //
  std::string encode_buffer_;
  std::chrono::nanoseconds next_heartbeat_ = {};
};

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
