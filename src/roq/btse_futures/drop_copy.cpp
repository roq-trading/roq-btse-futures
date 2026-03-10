/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/drop_copy.hpp"

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/btse_futures/json/map.hpp"
#include "roq/btse_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const SUPPORTS = Mask{
    SupportType::ORDER,
    SupportType::TRADE,
    SupportType::FUNDS,
    SupportType::POSITION,
};

auto const PING = "ping"sv;

size_t const MAX_DECODE_BUFFER_DEPTH = 1;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.ws.uri;
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = settings.net.connection_timeout,
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = settings.ws.ping_freq,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::socket::Client::create(handler, context, config, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

DropCopy::DropCopy(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
          .heartbeat = create_metrics(shared.settings, name_, "heartbeat"sv),
      },
      account_{account}, shared_{shared} {
}

bool DropCopy::ready() const {
  return (*connection_).ready();
}

void DropCopy::operator()(Event<Start> const &) {
  (*connection_).start();
}

void DropCopy::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void DropCopy::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      (*connection_).send_text(PING);
    }
  }
}

void DropCopy::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY)
      .write(latency_.heartbeat, metrics::Type::LATENCY);
}

// web::socket::Client::Handler

void DropCopy::operator()(web::socket::Client::Connected const &) {
  assert(logon_timeout_.count() == 0);
  auto now = clock::get_system();
  logon_timeout_ = now + shared_.settings.ws.request_timeout;
}

void DropCopy::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  ready_ = false;
  (*this)(ConnectionStatus::DISCONNECTED);
  logon_timeout_ = {};
  next_ping_ = {};
}

void DropCopy::operator()(web::socket::Client::Ready const &) {
  login();
}

void DropCopy::operator()(web::socket::Client::Close const &) {
}

void DropCopy::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void DropCopy::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void DropCopy::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(ConnectionStatus connection_status, std::string_view const &reason) {
  connection_status_ = connection_status;
  TraceInfo trace_info;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = account_.name,
      .supports = SUPPORTS,
      .transport = Transport::TCP,
      .protocol = Protocol::WS,
      .encoding = {Encoding::JSON},
      .priority = Priority::PRIMARY,
      .connection_status = connection_status_,
      .reason = reason,
      .interface = (*connection_).get_interface(),
      .authority = (*connection_).get_current_authority(),
      .path = (*connection_).get_current_path(),
      .proxy = (*connection_).get_proxy(),
  };
  log::info("stream_status={}"sv, stream_status);
  create_trace_and_dispatch(handler_, trace_info, stream_status);
}

void DropCopy::login() {
  auto message = account_.create_ws_login("/ws/futures"sv);
  (*connection_).send_text(message);
}

void DropCopy::subscribe() {
  subscribe("positionsV3"sv);
  // subscribe("allPositionV4"sv);  // note! looks like snapshot -- doesn't notify zero position
  subscribe("notificationApiV4"sv);
  subscribe("fillsV2"sv);
}

void DropCopy::subscribe(std::string_view const &topic) {
  log::info(R"(Subscribe topic="{}")"sv, topic);
  auto message = fmt::format(
      R"({{)"
      R"("op":"subscribe",)"
      R"("args":["{}"])"
      R"(}})"sv,
      topic);
  log::debug("message={}"sv, message);
  (*connection_).send_text(message);
}

void DropCopy::parse(std::string_view const &message) {
  profile_.parse([&]() {
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      TraceInfo trace_info;
      if (!json::Parser::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

// json::Parser::Handler

void DropCopy::operator()(Trace<json::Pong> const &) {
}

void DropCopy::operator()(Trace<json::Subscribe> const &) {
}

void DropCopy::operator()(Trace<json::TradeHistory> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::SnapshotL1> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::Update> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<json::Login> const &event) {
  auto &[trace_info, login] = event;
  log::info<2>("login={}"sv, login);
  if (!login.success) {
    log::fatal("Unexpected: login={}"sv, login);
  }
  subscribe();
  ready_ = true;
  (*this)(ConnectionStatus::READY);
}

void DropCopy::operator()(Trace<json::Positions> const &event) {
  auto &[trace_info, positions] = event;
  log::info<2>("positions={}"sv, positions);
  auto strip_symbol_from_market_name = [](auto &market_name) {
    auto pos = market_name.find_last_of('-');
    return market_name.substr(0, pos);
  };
  for (auto &item : positions.data) {
    auto long_quantity = [&]() -> double {
      switch (item.position_direction) {
        using enum json::PositionDirection::type_t;
        case UNDEFINED_INTERNAL:
          break;
        case UNKNOWN_INTERNAL:
          break;
        case LONG:
          return item.total_contracts;
        case SHORT:
          break;
      }
      return NaN;
    }();
    auto short_quantity = [&]() -> double {
      switch (item.position_direction) {
        using enum json::PositionDirection::type_t;
        case UNDEFINED_INTERNAL:
          break;
        case UNKNOWN_INTERNAL:
          break;
        case LONG:
          break;
        case SHORT:
          return item.total_contracts;
      }
      return NaN;
    }();
    auto position_update = PositionUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = strip_symbol_from_market_name(item.market_name),  // note!
        .margin_mode = {},                                          // margin_type_name ???
        .external_account = {},
        .long_quantity = long_quantity,
        .short_quantity = short_quantity,
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(handler_, trace_info, position_update, true);
  }
}

// note! not using this because we don't get any (real) update when the position goes to zero
void DropCopy::operator()(Trace<json::AllPosition> const &event) {
  auto &[trace_info, all_position] = event;
  log::info<2>("all_position={}"sv, all_position);
}

void DropCopy::operator()(Trace<json::Notification> const &event) {
  auto &[trace_info, notification] = event;
  log::info<2>("notification={}"sv, notification);
  for (auto &item : notification.data) {
    auto exchange_or_request_id = [&]() -> std::string_view {
      if (std::empty(item.cl_order_id)) {
        return item.order_id;
      }
      return item.cl_order_id;
    }();
    auto order_status = map(item.status).template get<OrderStatus>();
    auto has_last_traded = std::isnan(item.filled_size) || utils::compare(item.filled_size, 0.0) == 0;
    auto last_traded_quantity = has_last_traded ? item.filled_size : NaN;
    auto last_traded_price = has_last_traded ? item.price : NaN;  // XXX wrong
    auto last_liquidity = [&]() -> Liquidity {
      if (has_last_traded) {
        return item.maker ? Liquidity::MAKER : Liquidity::TAKER;
      }
      return {};
    }();
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
        .create_time_utc = {},
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
        .last_traded_quantity = last_traded_quantity,
        .last_traded_price = last_traded_price,
        .last_liquidity = last_liquidity,
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = UpdateType::INCREMENTAL,
        .sending_time_utc = item.timestamp,
    };
    if (shared_.update_order(exchange_or_request_id, stream_id_, trace_info, order_update, [&]([[maybe_unused]] auto &order) {})) {
    } else {
      log::warn("*** EXTERNAL ORDER *** ({} / {})"sv, item.order_id, exchange_or_request_id);
    }
  }
}

void DropCopy::operator()(Trace<json::Fills> const &event) {
  auto &[trace_info, fills] = event;
  log::info<2>("fills={}"sv, fills);
  for (auto &item : fills.data) {
    auto exchange_or_request_id = [&]() -> std::string_view {
      if (std::empty(item.cl_order_id)) {
        return item.order_id;
      }
      return item.cl_order_id;
    }();
    auto liquidity = item.maker ? Liquidity::MAKER : Liquidity::TAKER;
    auto fill = Fill{
        .exchange_time_utc = item.timestamp,
        .external_trade_id = item.trade_id,
        .quantity = item.size,
        .price = item.price,
        .liquidity = liquidity,
        .commission_amount = item.fee_amount,
        .commission_currency = item.fee_currency,
        .base_amount = NaN,
        .quote_amount = NaN,
        .profit_loss_amount = NaN,
    };
    auto trade_update = TradeUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .order_id = {},
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = map(item.side),
        .position_effect = {},
        .margin_mode = {},
        .create_time_utc = item.timestamp,
        .update_time_utc = item.timestamp,
        .external_account = {},
        .external_order_id = item.order_id,
        .client_order_id = item.cl_order_id,
        .fills = {&fill, 1},
        .routing_id = {},
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = item.timestamp,
        .exchange_sequence = utils::safe_cast(item.serial_id),
        .sending_time_utc = {},
        .user = {},
        .strategy_id = {},
    };
    create_trace_and_dispatch(handler_, trace_info, trade_update, true, SOURCE_NONE, exchange_or_request_id);
  }
}

}  // namespace btse_futures
}  // namespace roq
