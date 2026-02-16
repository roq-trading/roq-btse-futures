/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/order_entry.hpp"

#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/btse_futures/json/encoder.hpp"
#include "roq/btse_futures/json/error.hpp"
#include "roq/btse_futures/json/map.hpp"
#include "roq/btse_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "om"sv;

auto const SUPPORTS = Mask{
    SupportType::CREATE_ORDER,
    SupportType::MODIFY_ORDER,
    SupportType::CANCEL_ORDER,
    SupportType::ORDER_ACK,
    SupportType::FUNDS,
    SupportType::POSITION,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;

int32_t STATUS_REJECTED = 8;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.rest.uri;
  auto config = web::rest::Client::Config{
      // connection
      .interface = {},
      .proxy = settings.rest.proxy,
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = {},
      .disconnect_on_idle_timeout = {},
      .connection = web::http::Connection::KEEP_ALIVE,
      // request
      .allow_pipelining = true,
      .request_timeout = settings.rest.request_timeout,
      // response
      .suspend_on_retry_after = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .ping_frequency = settings.rest.ping_freq,
      .ping_path = settings.rest.ping_path,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::rest::Client::create(handler, context, config);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

OrderEntry::OrderEntry(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, bool master)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)}, master_{master},
      connection_{create_connection(*this, shared.settings, context)}, decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .position_mode = create_metrics(shared.settings, name_, "position_mode"sv),
          .position_mode_ack = create_metrics(shared.settings, name_, "position_mode_ack"sv),
          .wallet = create_metrics(shared.settings, name_, "wallet"sv),
          .wallet_ack = create_metrics(shared.settings, name_, "wallet_ack"sv),
          .positions = create_metrics(shared.settings, name_, "positions"sv),
          .positions_ack = create_metrics(shared.settings, name_, "positions_ack"sv),
          .open_orders = create_metrics(shared.settings, name_, "open_orders"sv),
          .open_orders_ack = create_metrics(shared.settings, name_, "open_orders_ack"sv),
          .fill_history = create_metrics(shared.settings, name_, "fill_history"sv),
          .fill_history_ack = create_metrics(shared.settings, name_, "fill_history_ack"sv),
          .place_order = create_metrics(shared.settings, name_, "place_order"sv),
          .place_order_ack = create_metrics(shared.settings, name_, "place_order_ack"sv),
          .modify_order = create_metrics(shared.settings, name_, "modify_order"sv),
          .modify_order_ack = create_metrics(shared.settings, name_, "modify_order_ack"sv),
          .cancel_order = create_metrics(shared.settings, name_, "cancel_order"sv),
          .cancel_order_ack = create_metrics(shared.settings, name_, "cancel_order_ack"sv),
          .cancel_all_orders = create_metrics(shared.settings, name_, "cancel_all_orders"sv),
          .cancel_all_orders_ack = create_metrics(shared.settings, name_, "cancel_all_orders_ack"sv),
          .countdown_cancel_all = create_metrics(shared.settings, name_, "countdown_cancel_all"sv),
          .countdown_cancel_all_ack = create_metrics(shared.settings, name_, "countdown_cancel_all_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared}, download_{shared.settings.rest.request_timeout, [this](auto state) { return download(state); }} {
}

void OrderEntry::operator()(Event<Start> const &) {
  (*connection_).start();
}

void OrderEntry::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void OrderEntry::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (!ready()) {
    return;
  }
  if (master_ && shared_.settings.rest.cancel_on_disconnect && next_heartbeat_ < now) {
    next_heartbeat_ = now + (shared_.settings.rest.ping_freq / 3);
    cancel_all_after();
  }
}

void OrderEntry::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.position_mode, metrics::Type::PROFILE)
      .write(profile_.position_mode_ack, metrics::Type::PROFILE)
      .write(profile_.wallet, metrics::Type::PROFILE)
      .write(profile_.wallet_ack, metrics::Type::PROFILE)
      .write(profile_.positions, metrics::Type::PROFILE)
      .write(profile_.positions_ack, metrics::Type::PROFILE)
      .write(profile_.open_orders, metrics::Type::PROFILE)
      .write(profile_.open_orders_ack, metrics::Type::PROFILE)
      .write(profile_.fill_history, metrics::Type::PROFILE)
      .write(profile_.fill_history_ack, metrics::Type::PROFILE)
      .write(profile_.place_order, metrics::Type::PROFILE)
      .write(profile_.place_order_ack, metrics::Type::PROFILE)
      .write(profile_.modify_order, metrics::Type::PROFILE)
      .write(profile_.modify_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_order, metrics::Type::PROFILE)
      .write(profile_.cancel_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders_ack, metrics::Type::PROFILE)
      .write(profile_.countdown_cancel_all, metrics::Type::PROFILE)
      .write(profile_.countdown_cancel_all_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

uint16_t OrderEntry::operator()(
    Event<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &, std::string_view const &request_id) {
  create_order(event, order, request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  amend_order(event, order, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  cancel_order(event, order, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  cancel_all_orders(event, request_id);
  return stream_id_;
}

// web::rest::Client::Handler

void OrderEntry::operator()(Trace<web::rest::Client::Connected> const &) {
  if (download_.downloading()) {
    download_.bump();
  } else {
    (*this)(ConnectionStatus::DOWNLOADING);
    download_.begin();
  }
}

void OrderEntry::operator()(Trace<web::rest::Client::Disconnected> const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  if (!download_.downloading()) {
    download_.reset();
  }
  next_heartbeat_ = {};
}

void OrderEntry::operator()(Trace<web::rest::Client::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void OrderEntry::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = account_.name,
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::HTTP,
        .encoding = {Encoding::JSON},
        .priority = Priority::PRIMARY,
        .connection_status = status_,
        .interface = (*connection_).get_interface(),
        .authority = (*connection_).get_current_authority(),
        .path = (*connection_).get_current_path(),
        .proxy = (*connection_).get_proxy(),
    };
    log::info("stream_status={}"sv, stream_status);
    create_trace_and_dispatch(handler_, trace_info, stream_status);
  }
}

uint32_t OrderEntry::download(OrderEntryState state) {
  switch (state) {
    using enum OrderEntryState;
    case UNDEFINED:
      assert(false);
      break;
    case POSITION_MODE:
      get_position_mode();
      return 1;
    case WALLET:
      get_wallet();
      return 1;
    case POSITIONS:
      get_positions();
      return 1;
    case OPEN_ORDERS:
      get_open_orders();
      return 1;
    case FILL_HISTORY:
      if (shared_.settings.rest.download_fills_begin.count()) {
        // get_fill_history();
        // return 1;
        return 0;
      } else {
        return 0;
      }
    case DONE:
      (*this)(ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// position-mode

void OrderEntry::get_position_mode() {
  profile_.position_mode([&]() {
    auto path = shared_.api.order_management.position_mode;
    auto headers = account_.create_headers(path);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("position-mode"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_position_mode_ack(event, sequence);
    });
  });
}

void OrderEntry::get_position_mode_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = OrderEntryState::POSITION_MODE;
  profile_.position_mode_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::GetPositionModeAck position_mode_ack{body, decode_buffer_};
        Trace event{trace_info, position_mode_ack};
        (*this)(event);
        download_.check(state);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::GetPositionModeAck> const &event) {
  auto &[trace_info, position_mode_ack] = event;
  log::info<4>("position_mode_ack={}"sv, position_mode_ack);
}

// wallet

void OrderEntry::get_wallet() {
  profile_.wallet([&]() {
    auto path = shared_.api.order_management.wallet;
    auto headers = account_.create_headers(path);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("wallet"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_wallet_ack(event, sequence);
    });
  });
}

void OrderEntry::get_wallet_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = OrderEntryState::WALLET;
  profile_.wallet_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::GetWalletAck wallet_ack{body, decode_buffer_};
        Trace event{trace_info, wallet_ack};
        (*this)(event);
        download_.check(state);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::GetWalletAck> const &event) {
  auto &[trace_info, wallet_ack] = event;
  log::info<4>("wallet_ack={}"sv, wallet_ack);
}

// positions

void OrderEntry::get_positions() {
  profile_.positions([&]() {
    auto path = shared_.api.order_management.positions;
    auto headers = account_.create_headers(path);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("positions"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_positions_ack(event, sequence);
    });
  });
}

void OrderEntry::get_positions_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = OrderEntryState::POSITIONS;
  profile_.positions_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::GetPositionsAck positions_ack{body, decode_buffer_};
        Trace event{trace_info, positions_ack};
        (*this)(event);
        download_.check(state);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::GetPositionsAck> const &event) {
  auto &[trace_info, positions_ack] = event;
  log::info<4>("positions_ack={}"sv, positions_ack);
  for (auto &item : positions_ack.data) {
    auto long_quantity = [&]() -> double {
      switch (item.side) {
        using enum json::Side::type_t;
        case UNDEFINED_INTERNAL:
          break;
        case UNKNOWN_INTERNAL:
          break;
        case BUY:
          return item.size;
        case SELL:
          break;
      }
      return NaN;
    }();
    auto short_quantity = [&]() -> double {
      switch (item.side) {
        using enum json::Side::type_t;
        case UNDEFINED_INTERNAL:
          break;
        case UNKNOWN_INTERNAL:
          break;
        case BUY:
          break;
        case SELL:
          return item.size;
      }
      return NaN;
    }();
    auto position_update = PositionUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .margin_mode = {},  // margin_type_name ???
        .external_account = {},
        .long_quantity = long_quantity,
        .short_quantity = short_quantity,
        .update_type = UpdateType::SNAPSHOT,
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(handler_, trace_info, position_update, true);
  }
}

// open-orders

void OrderEntry::get_open_orders() {
  profile_.open_orders([&]() {
    auto path = shared_.api.order_management.open_orders;
    auto headers = account_.create_headers(path);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("open_orders"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_open_orders_ack(event, sequence);
    });
  });
}

void OrderEntry::get_open_orders_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = OrderEntryState::OPEN_ORDERS;
  profile_.open_orders_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::GetOpenOrdersAck open_orders_ack{body, decode_buffer_};
        Trace event{trace_info, open_orders_ack};
        (*this)(event);
        download_.check(state);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::GetOpenOrdersAck> const &event) {
  auto &[trace_info, open_orders_ack] = event;
  log::info<4>("open_orders_ack={}"sv, open_orders_ack);
  for (auto &item : open_orders_ack.data) {
    auto order_status = map(item.order_state).template get<OrderStatus>();
    auto order_update = server::oms::OrderUpdate{
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = map(item.side),
        .position_effect = {},
        .margin_mode = {},
        .max_show_quantity = NaN,
        .order_type = map(item.order_type),
        .time_in_force = map(item.time_in_force),
        .execution_instructions = {},
        .create_time_utc = item.timestamp,
        .update_time_utc = item.timestamp,
        .external_account = {},
        .external_order_id = item.order_id,
        .client_order_id = item.cl_order_id,
        .order_status = order_status,
        .error = {},
        .text = {},
        .quantity = item.current_order_size,
        .price = item.price,
        .stop_price = NaN,
        .leverage = NaN,
        .remaining_quantity = item.remaining_size,
        .traded_quantity = item.total_filled_size,
        .average_traded_price = item.avg_filled_price,
        .last_traded_quantity = NaN,
        .last_traded_price = NaN,
        .last_liquidity = {},
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = UpdateType::SNAPSHOT,
        .sending_time_utc = item.timestamp,
    };
    Trace event_2{trace_info, order_update};
    (*this)(event_2, item.cl_order_id);
  }
}
/*
// fill_history
void OrderEntry::get_fill_history() {
  assert(shared_.settings.rest.download_fills_begin.count() > 0);
  profile_.fill_history([&]() {
    auto now = clock::get_realtime();
    auto start_time = std::chrono::duration_cast<std::chrono::milliseconds>(now - shared_.settings.rest.download_fills_begin);
    auto method = web::http::Method::GET;
    auto path = shared_.api.order_management.fill_history;
    auto query = fmt::format("?category={}&startTime={}"sv, shared_.api.category, start_time.count());
    log::warn("DEBUG query={}"sv, query);
    auto headers = account_.create_headers(method, path, query, {});
    auto request = web::rest::Request{
        .method = method,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("fill_history"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_fill_history_ack(event, sequence);
    });
  });
}

void OrderEntry::get_fill_history_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = OrderEntryState::FILL_HISTORY;
  profile_.fill_history_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::FillHistory fill_history{body, decode_buffer_};
        if (fill_history.code == 0) {
          Trace event{trace_info, fill_history};
          (*this)(event);
          download_.check(state);
        } else {
          handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(fill_history.code), fill_history.msg);
        }
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::FillHistory> const &event) {
  auto &[trace_info, fill_history] = event;
  log::info<4>("fill_history={}"sv, fill_history);
  std::string_view symbol, order_id, client_oid;
  json::Side side = {};
  json::TradeSide trade_side = {};
  std::chrono::nanoseconds created_time = {};
  std::chrono::nanoseconds updated_time = {};
  auto dispatch = [&]() {
    if (!std::empty(shared_.fills)) {
      auto trade_update = TradeUpdate{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = shared_.settings.exchange,
          .symbol = symbol,
          .side = map(side),
          .position_effect = map(trade_side),
          .margin_mode = {},  // XXX FIXME TODO from asset_info[symbol]
          .quantity_type = {},
          .create_time_utc = created_time,
          .update_time_utc = updated_time,
          .external_account = {},
          .external_order_id = order_id,
          .client_order_id = client_oid,
          .fills = shared_.fills,
          .routing_id = {},
          .update_type = UpdateType::SNAPSHOT,
          .sending_time_utc = fill_history.request_time,
          .user = {},
          .strategy_id = {},
      };
      create_trace_and_dispatch(handler_, trace_info, trade_update, true, SOURCE_NONE, client_oid);
      log::warn("DEBUG trade_update={}"sv, trade_update);
      shared_.fills.clear();
    }
  };
  shared_.fills.clear();
  for (auto &item : fill_history.data.list) {
    log::warn("DEBUG item={}"sv, item);
    if (item.symbol != symbol || item.order_id != order_id || item.client_oid != client_oid || item.side != side || item.trade_side != trade_side) {
      dispatch();
      symbol = item.symbol;
      order_id = item.order_id;
      client_oid = item.client_oid;
      side = item.side;
      trade_side = item.trade_side, created_time = {};
      updated_time = {};
    }
    std::string_view fee_coin;
    double fee = 0.0;
    bool please_report = false;
    for (auto &item_2 : item.fee_detail) {
      if (!std::isnan(item_2.fee)) {
        fee += item_2.fee;
      }
      if (!std::empty(item_2.fee_coin)) {
        if (std::empty(fee_coin)) {
          fee_coin = item_2.fee_coin;
        } else if (item_2.fee_coin != fee_coin) {
          log::warn(R"(fee_coin="{}"!="{}")"sv, item_2.fee_coin, fee_coin);
        }
      }
    }
    if (please_report) {
      log::warn("*** PLEASE REPORT *** fill={}"sv, item);
    }
    auto fill = Fill{
        .exchange_time_utc = item.created_time,
        .external_trade_id = item.exec_id,
        .quantity = item.exec_qty,
        .price = item.exec_price,
        .liquidity = map(item.trade_scope),
        .commission_amount = fee,
        .commission_currency = fee_coin,
        .base_amount = NaN,   // XXX FIXME TODO
        .quote_amount = NaN,  // XXX FIXME TODO
        .profit_loss_amount = NaN,
    };
    shared_.fills.emplace_back(std::move(fill));
    if (created_time < item.created_time) {
      created_time = item.created_time;
    }
    if (updated_time < item.updated_time) {
      updated_time = item.updated_time;
    }
  }
  dispatch();
}
*/
// create-order

void OrderEntry::create_order(Event<CreateOrder> const &event, server::oms::Order const &order, std::string_view const &request_id) {
  profile_.place_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, create_order] = event;
    auto path = shared_.api.order_management.create_order;
    auto body = json::Encoder::place_order(encode_buffer_, create_order, order, request_id);
    log::warn(R"(DEBUG body="{}")"sv, body);
    auto headers = account_.create_headers(path, body);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = create_order.order_id]([[maybe_unused]] auto &request_id, auto &response) {
      uint32_t version = 1;
      TraceInfo trace_info;
      Trace event{trace_info, response};
      create_order_ack(event, user_id, order_id, version);
    };
    (*connection_)(request_id, request, callback);
  });
}

void OrderEntry::create_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.place_order_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CREATE_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      json::PlaceOrderAck place_order_ack{body, decode_buffer_};
      size_t failed = 0;
      for (auto &item : place_order_ack.data) {
        if (item.status != STATUS_REJECTED) {
          continue;
        }
        ++failed;
        handle_error(Origin::GATEWAY, RequestStatus::REJECTED, Error::UNKNOWN, item.message);
      }
      if (failed < std::size(place_order_ack.data)) {
        Trace event_2{event, place_order_ack};
        (*this)(event_2, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<json::PlaceOrderAck> const &event, [[maybe_unused]] uint8_t user_id, [[maybe_unused]] uint64_t order_id, [[maybe_unused]] uint32_t version) {
  auto &[trace_info, place_order_ack] = event;
  log::info<2>("place_order_ack={}"sv, place_order_ack);
  log::warn("DEBUG place_order_ack={}"sv, place_order_ack);
  // XXX FIXME TODO reject => response
}

// amend-order

void OrderEntry::amend_order(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.modify_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, modify_order] = event;
    auto path = shared_.api.order_management.amend_order;
    auto body = json::Encoder::modify_order(encode_buffer_, modify_order, order, request_id);
    log::warn(R"(DEBUG body="{}")"sv, body);
    auto headers = account_.create_headers(path, body);
    auto request = web::rest::Request{
        .method = web::http::Method::PUT,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = modify_order.order_id, version = modify_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      amend_order_ack(event, user_id, order_id, version);
    };
    (*connection_)(request_id, request, callback);
  });
}

void OrderEntry::amend_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.modify_order_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::MODIFY_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      json::ModifyOrderAck modify_order_ack{body, decode_buffer_};
      size_t failed = 0;
      for (auto &item : modify_order_ack.data) {
        if (item.status != STATUS_REJECTED) {
          continue;
        }
        ++failed;
        handle_error(Origin::GATEWAY, RequestStatus::REJECTED, Error::UNKNOWN, item.message);
      }
      if (failed < std::size(modify_order_ack.data)) {
        Trace event_2{event, modify_order_ack};
        (*this)(event_2, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<json::ModifyOrderAck> const &event, [[maybe_unused]] uint8_t user_id, [[maybe_unused]] uint64_t order_id, [[maybe_unused]] uint32_t version) {
  auto &[trace_info, modify_order_ack] = event;
  log::info<2>("modify_order_ack={}"sv, modify_order_ack);
  log::warn("DEBUG modify_order_ack={}"sv, modify_order_ack);
  // XXX FIXME TODO reject => response
}

// cancel-order

void OrderEntry::cancel_order(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.cancel_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_order] = event;
    auto path = shared_.api.order_management.cancel_order;
    auto query = json::Encoder::cancel_order(encode_buffer_, cancel_order, order, request_id);
    log::warn(R"(DEBUG query="{}")"sv, query);
    auto headers = account_.create_headers(path);
    auto request = web::rest::Request{
        .method = web::http::Method::DELETE,
        .path = path,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = cancel_order.order_id, version = cancel_order.version](
                        [[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      cancel_order_ack(event, user_id, order_id, version);
    };
    (*connection_)(request_id, request, callback);
  });
}

void OrderEntry::cancel_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.cancel_order_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CANCEL_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      Trace event_2{event, response};
      (*this)(event_2, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG {}"sv, body);
      json::CancelOrderAck cancel_order_ack{body, decode_buffer_};
      size_t failed = 0;
      for (auto &item : cancel_order_ack.data) {
        if (item.status != STATUS_REJECTED) {
          continue;
        }
        ++failed;
        handle_error(Origin::GATEWAY, RequestStatus::REJECTED, Error::UNKNOWN, item.message);
      }
      if (failed < std::size(cancel_order_ack.data)) {
        Trace event_2{event, cancel_order_ack};
        (*this)(event_2, user_id, order_id, version);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<json::CancelOrderAck> const &event, [[maybe_unused]] uint8_t user_id, [[maybe_unused]] uint64_t order_id, [[maybe_unused]] uint32_t version) {
  auto &[trace_info, cancel_order_ack] = event;
  log::info<2>("cancel_order_ack={}"sv, cancel_order_ack);
  log::warn("DEBUG cancel_order_ack={}"sv, cancel_order_ack);
  // XXX FIXME TODO reject => response
}

// cancel-all-orders

void OrderEntry::cancel_all_orders(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  profile_.cancel_all_orders([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_all_orders] = event;
    auto helper = [&](auto &symbol) {
      auto path = shared_.api.order_management.cancel_all_orders;
      auto query = json::Encoder::cancel_all_orders(encode_buffer_, cancel_all_orders, request_id, symbol);
      log::warn(R"(DEBUG query="{}")"sv, query);
      auto headers = account_.create_headers(path);
      auto request = web::rest::Request{
          .method = web::http::Method::DELETE,
          .path = path,
          .query = query,
          .accept = web::http::Accept::APPLICATION_JSON,
          .content_type = web::http::ContentType::APPLICATION_JSON,
          .headers = headers,
          .body = {},
          .quality_of_service = {},
      };
      auto callback = [this](auto &request_id, auto &response) {
        TraceInfo trace_info;
        Trace event{trace_info, response};
        cancel_all_orders_ack(event, request_id);
      };
      (*connection_)(request_id, request, callback);
    };
    if (shared_.dispatcher.get_all_order_symbols(helper, account_.name)) {
    } else {
      log::warn("*** NOT POSSIBLE TO CANCEL ALL OPEN ORDERS (NO SYMBOLS) ***"sv);
    }
  });
}

void OrderEntry::cancel_all_orders_ack(Trace<web::rest::Response> const &event, std::string_view const &request_id) {
  profile_.cancel_all_orders_ack([&]() {
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto cancel_all_orders_ack = CancelAllOrdersAck{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = shared_.settings.exchange,
          .symbol = {},
          .side = {},
          .origin = Origin::EXCHANGE,
          .request_status = RequestStatus::REJECTED,
          .error = error,
          .text = text,
          .request_id = {},  // ???
          .external_account = {},
          .number_of_affected_orders = {},
          .round_trip_latency = {},
          .user = {},
          .strategy_id = {},
      };
      TraceInfo trace_info;
      Trace event_2{trace_info, cancel_all_orders_ack};
      shared_(event_2);
    };
    auto handle_success = [&](auto &body) {
      json::CancelAllOrdersAck cancel_all_orders_ack{body, decode_buffer_};
      Trace event_2{event, cancel_all_orders_ack};
      (*this)(event_2, request_id);
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<json::CancelAllOrdersAck> const &event, [[maybe_unused]] std::string_view const &request_id) {
  auto &[trace_info, cancel_all_orders_ack] = event;
  log::info<2>("cancel_all_orders_ack={}"sv, cancel_all_orders_ack);
  log::warn("DEBUG cancel_all_orders_ack={}"sv, cancel_all_orders_ack);
  // XXX FIXME TODO reject => response
}

// cancel-all-after

void OrderEntry::cancel_all_after() {
  profile_.countdown_cancel_all([&]() {
    auto path = shared_.api.order_management.cancel_all_after;
    auto body = json::Encoder::cancel_all_after(encode_buffer_, utils::safe_cast{shared_.settings.rest.ping_freq});
    auto headers = account_.create_headers(path, body);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = path,
        .query = {},
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_JSON,
        .headers = headers,
        .body = body,
        .quality_of_service = {},
    };
    auto callback = [this]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      cancel_all_after_ack(event);
    };
    (*connection_)("cancel-all-after"sv, request, callback);
  });
}

void OrderEntry::cancel_all_after_ack(Trace<web::rest::Response> const &event) {
  profile_.countdown_cancel_all_ack([&]() {
    auto &[trace_info, response] = event;
    auto [status, category, body] = response.result();
    if (status != web::http::Status::OK) {
      log::error("response={}"sv, response);
    }
  });
}

// helpers

void OrderEntry::process_response(web::rest::Response const &response, auto error_handler, auto success_handler) {
  try {
    auto [status, category, body] = response.result();
    switch (category) {
      using enum web::http::Category;
      case UNKNOWN:
      case INFORMATIONAL_RESPONSE:
        response.expect(web::http::Status::OK);  // throws
        break;
      case SUCCESS:
        success_handler(body);
        break;
      case REDIRECTION:
        log::fatal("Unexpected: URL is being redirected"sv);
      case CLIENT_ERROR:
        log::warn("{}"sv, body);
        switch (status) {
          using enum web::http::Status;
          case FORBIDDEN:            // 403
          case I_AM_A_TEAPOT:        // 418
          case TOO_MANY_REQUESTS: {  // 429
            auto message = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, message);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default:
            if (std::empty(body)) {
              auto message = fmt::format("{}"sv, status);
              error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
            } else {
              json::Error error{body};
              error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(error.error_code), error.message);
            }
        }
        break;
      case SERVER_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
    }
  } catch (server::oms::Exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(e.origin, e.status, e.error, e.what());
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

template <typename... Args>
void OrderEntry::operator()(Trace<server::oms::Response> const &event, uint8_t user_id, uint64_t order_id, Args &&...args) {
  auto &[trace_info, response] = event;
  if (shared_.update_order(user_id, order_id, stream_id_, trace_info, response, std::forward<Args>(args)..., []([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("Did not find order: user_id={}, order_id={}"sv, user_id, order_id);
  }
}

void OrderEntry::operator()(Trace<server::oms::OrderUpdate> const &event, std::string_view const &client_order_id) {
  auto &[trace_info, order_update] = event;
  if (shared_.update_order(client_order_id, stream_id_, trace_info, order_update, [&]([[maybe_unused]] auto &order) {})) {
  } else {
    log::warn("*** EXTERNAL ORDER ***"sv);
  }
}

}  // namespace btse_futures
}  // namespace roq
