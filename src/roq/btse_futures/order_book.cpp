/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/btse_futures/order_book.hpp"

#include "roq/logging.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/btse_futures/json/map.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "ob"sv;

auto const SUPPORTS = Mask{
    SupportType::TOP_OF_BOOK,
    SupportType::MARKET_BY_PRICE,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;

auto const PING = "ping"sv;

uint16_t const MAX_DEPTH = 50;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.ws.uri_oss;
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
      .query = {},
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

OrderBook::OrderBook(Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared, size_t index)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, index_{index}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .snapshot_l1 = create_metrics(shared.settings, name_, "snapshot_l1"sv),
          .update = create_metrics(shared.settings, name_, "update"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared} {
}

void OrderBook::operator()(Event<Start> const &) {
  (*connection_).start();
}

void OrderBook::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void OrderBook::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    if (next_ping_ < now) {
      next_ping_ = now + shared_.settings.ws.ping_freq;
      ping(now);
    }
  }
}

void OrderBook::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.snapshot_l1, metrics::Type::PROFILE)
      .write(profile_.update, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void OrderBook::subscribe(size_t start_from) {
  if (ready()) {
    subscribe(shared_.symbols.get_slice(index_, start_from));
  }
}

void OrderBook::operator()(web::socket::Client::Connected const &) {
}

void OrderBook::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
}

void OrderBook::operator()(web::socket::Client::Ready const &) {
  (*this)(ConnectionStatus::READY);
  subscribe();
}

void OrderBook::operator()(web::socket::Client::Close const &) {
}

void OrderBook::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void OrderBook::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void OrderBook::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(ConnectionStatus status) {
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

void OrderBook::ping(std::chrono::nanoseconds now) {
  (*connection_).send_text(PING);
}

void OrderBook::subscribe(std::span<Symbol const> const &symbols) {
  if (std::empty(symbols)) {
    return;
  }
  subscribe(symbols, "snapshotL1"sv);
  subscribe(symbols, "update"sv, 0);
}

void OrderBook::subscribe(std::span<Symbol const> const &symbols, std::string_view const &channel) {
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

void OrderBook::subscribe(std::span<Symbol const> const &symbols, std::string_view const &channel, uint32_t group) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
    auto message = fmt::format(
        R"({{)"
        R"("op":"subscribe",)"
        R"("args":[)"
        R"("{}:{}_{}")"
        R"(])"
        R"(}})"sv,
        channel,
        item,
        group);
    (*connection_).send_text(message);
  }
}

void OrderBook::parse(std::string_view const &message) {
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

void OrderBook::operator()(Trace<json::Pong> const &) {
}

void OrderBook::operator()(Trace<json::TradeHistory> const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(Trace<json::SnapshotL1> const &event) {
  profile_.snapshot_l1([&]() {
    auto &[trace_info, snapshot_l1] = event;
    log::info<3>("snapshot_l1={}"sv, snapshot_l1);
    auto &data = snapshot_l1.data;
    if (std::size(data.bids) > 1 || std::size(data.asks) > 1) {
      log::fatal("Unexpected: snapshot_l1={}"sv, snapshot_l1);
    }
    auto price_helper = [](auto &value) -> double { return std::empty(value) ? NaN : value[0].price; };
    auto size_helper = [](auto &value) -> double { return std::empty(value) ? NaN : value[0].size; };
    auto top_of_book = TopOfBook{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = data.symbol,
        .layer{
            .bid_price = price_helper(data.bids),
            .bid_quantity = size_helper(data.bids),
            .ask_price = price_helper(data.asks),
            .ask_quantity = size_helper(data.asks),
        },
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = data.timestamp,
        .exchange_sequence = {},
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(handler_, trace_info, top_of_book, true);
  });
}

void OrderBook::operator()(Trace<json::Update> const &event) {
  profile_.update([&]() {
    auto &[trace_info, update] = event;
    log::info<3>("update={}"sv, update);
    auto helper = [&](auto &result, auto &item) {
      auto mbp_update = MBPUpdate{
          .price = item.price,
          .quantity = item.size,
          .implied_quantity = NaN,
          .number_of_orders = {},
          .update_action = {},
          .price_level = {},
      };
      result.emplace_back(std::move(mbp_update));
    };
    auto &data = update.data;
    auto &bids = shared_.bids;
    auto &asks = shared_.asks;
    bids.clear();
    asks.clear();
    for (auto &item : data.bids) {
      helper(bids, item);
    }
    for (auto &item : data.asks) {
      helper(asks, item);
    }
    if (std::empty(bids) && std::empty(asks)) {
      return;
    }
    auto market_by_price_update = MarketByPriceUpdate{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = data.symbol,
        .bids = bids,
        .asks = asks,
        .update_type = map(data.type),
        .exchange_time_utc = data.timestamp,
        .exchange_sequence = utils::safe_cast{data.seq_num},
        .sending_time_utc = {},
        .price_precision = {},
        .quantity_precision = {},
        .max_depth = MAX_DEPTH,
        .checksum = {},
    };
    create_trace_and_dispatch(handler_, trace_info, market_by_price_update, true);
  });
}

void OrderBook::operator()(Trace<json::Login> const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(Trace<json::Account> const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(Trace<json::Position> const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(Trace<json::Order> const &) {
  log::fatal("Unexpected"sv);
}

void OrderBook::operator()(Trace<json::Fill> const &) {
  log::fatal("Unexpected"sv);
}

}  // namespace btse_futures
}  // namespace roq
