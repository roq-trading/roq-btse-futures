/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/market_data.hpp"

#include "roq/logging.hpp"

#include "roq/utils/update.hpp"

#include "roq/utils/charconv/to_string.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/btse_futures/json/map.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "md"sv;

auto const SUPPORTS = Mask{
    SupportType::TRADE_SUMMARY,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 1;

auto const PING = "ping"sv;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
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

MarketData::MarketData(Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared, size_t index)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, index_{index}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .trade_history = create_metrics(shared.settings, name_, "trade_history"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared} {
}

void MarketData::operator()(Event<Start> const &) {
  (*connection_).start();
}

void MarketData::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void MarketData::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      ping(now);
    }
  }
}

void MarketData::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.trade_history, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void MarketData::subscribe(size_t start_from) {
  if (ready()) {
    subscribe(shared_.symbols.get_slice(index_, start_from));
  }
}

// web::socket::Client::Handler

void MarketData::operator()(web::socket::Client::Connected const &) {
}

void MarketData::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
}

void MarketData::operator()(web::socket::Client::Ready const &) {
  (*this)(ConnectionStatus::READY);
  subscribe();
}

void MarketData::operator()(web::socket::Client::Close const &) {
}

void MarketData::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void MarketData::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void MarketData::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = {},
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::WS,
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

void MarketData::ping([[maybe_unused]] std::chrono::nanoseconds now) {
  (*connection_).send_text(PING);
}

void MarketData::subscribe(std::span<Symbol const> const &symbols) {
  if (std::empty(symbols)) {
    return;
  }
  subscribe(symbols, "tradeHistoryApi"sv);
}

void MarketData::subscribe(std::span<Symbol const> const &symbols, std::string_view const &channel) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
    auto message = fmt::format(
        R"({{)"
        R"("op":"subscribe",)"
        R"("args":[)"
        R"("{}:{}")"
        R"(])"
        R"(}})"sv,
        channel,
        item);
    (*connection_).send_text(message);
  }
}

void MarketData::parse(std::string_view const &message) {
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

void MarketData::operator()(Trace<json::Pong> const &) {
}

void MarketData::operator()(Trace<json::Subscribe> const &) {
}

// note! can't detect snapshot...
void MarketData::operator()(Trace<json::TradeHistory> const &event) {
  profile_.trade_history([&]() {
    auto &[trace_info, trade_history] = event;
    log::info<3>("trade_history={}"sv, trade_history);
    auto &trades = shared_.trades;
    trades.clear();
    std::string_view symbol;
    decltype(json::TradeHistoryDataItem::timestamp) timestamp = {};
    auto dispatch = [&]() {
      if (std::empty(trades)) {
        return;
      }
      auto trade_summary = TradeSummary{
          .stream_id = stream_id_,
          .exchange = shared_.settings.exchange,
          .symbol = symbol,
          .trades = trades,
          .exchange_time_utc = timestamp,
          .exchange_sequence = {},
          .sending_time_utc = {},
      };
      create_trace_and_dispatch(handler_, trace_info, trade_summary, true);
      trades.clear();
    };
    for (auto &item : trade_history.data) {
      if (item.symbol != symbol || item.timestamp != timestamp) {
        dispatch();
        symbol = item.symbol;
        timestamp = item.timestamp;
      }
      auto trade = Trade{
          .side = map(item.side),
          .price = item.price,
          .quantity = item.size,
          .trade_id = {},
          .taker_order_id = {},
          .maker_order_id = {},
      };
      utils::charconv::to_string(std::back_inserter(trade.trade_id), item.trade_id);
      trades.emplace_back(std::move(trade));
    }
    dispatch();
  });
}

void MarketData::operator()(Trace<json::SnapshotL1> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Update> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Login> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Positions> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::AllPosition> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Notification> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Fills> const &) {
  log::fatal("Unexpected"sv);
}

}  // namespace btse_futures
}  // namespace roq
