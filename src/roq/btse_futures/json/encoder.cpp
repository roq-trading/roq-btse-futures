/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/json/encoder.hpp"

#include <fmt/format.h>

#include "roq/decimal.hpp"

#include "roq/btse_futures/json/map.hpp"
#include "roq/btse_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace json {

// XXX FIXME TODO stop
std::string_view Encoder::place_order(
    std::string &buffer, CreateOrder const &create_order, server::oms::Order const &order, std::string_view const &request_id) {
  auto side = map(create_order.side).template get<json::Side>();
  auto type = map(create_order.order_type).template get<json::OrderType>();
  auto time_in_force = map(create_order.time_in_force).template get<json::TimeInForce>();
  auto post_only = [&]() { return create_order.execution_instructions.has(ExecutionInstruction::PARTICIPATE_DO_NOT_INITIATE); }();
  auto reduce_only = [&]() { return create_order.execution_instructions.has(ExecutionInstruction::DO_NOT_INCREASE); }();
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("symbol":"{}")"
      R"(,"side":"{}")"
      R"(,"type":"{}")"
      R"(,"postOnly":{})"
      R"(,"reduceOnly":{})"
      R"(,"size":{})"sv,
      create_order.symbol,
      side.as_raw_text(),
      type.as_raw_text(),
      post_only,
      reduce_only,
      Decimal{create_order.quantity, order.quantity_precision.precision});
  if (create_order.time_in_force != roq::TimeInForce::UNDEFINED) {
    fmt::format_to(std::back_inserter(buffer), R"(,"time_in_force":"{}")"sv, time_in_force.as_raw_text());
  }
  if (!std::isnan(create_order.price)) {
    fmt::format_to(std::back_inserter(buffer), R"(,"price":{})"sv, Decimal{create_order.price, order.price_precision.precision});
  }
  fmt::format_to(
      std::back_inserter(buffer),
      R"(,"clOrderID":"{}")"
      R"(}})"sv,
      request_id);
  return buffer;
}

std::string_view Encoder::modify_order(
    std::string &buffer, ModifyOrder const &modify_order, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("symbol":"{}")"sv,
      order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), R"(,"clOrderID":"{}")"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), R"(,"orderID":"{}")"sv, order.external_order_id);
  }
  auto has_quantity = !std::isnan(modify_order.quantity);
  auto has_price = !std::isnan(modify_order.price);
  if (has_quantity && has_price) {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"type":"ALL")"
        R"(,"orderSize":{})"
        R"(,"orderPrice":{})"sv,
        Decimal{modify_order.quantity, order.quantity_precision.precision},
        Decimal{modify_order.price, order.price_precision.precision});
  } else if (has_quantity) {
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"type":"SIZE")"
        R"(,"value":{})"sv,
        Decimal{modify_order.quantity, order.quantity_precision.precision});
  } else {
    assert(has_price);
    fmt::format_to(
        std::back_inserter(buffer),
        R"(,"type":"PRICE")"
        R"(,"value":{})"sv,
        Decimal{modify_order.price, order.price_precision.precision});
  }
  fmt::format_to(std::back_inserter(buffer), R"(}})"sv);
  return buffer;
}

std::string_view Encoder::cancel_order(
    std::string &buffer, CancelOrder const &, server::oms::Order const &order, [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "?symbol={}"sv, order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "&clOrderID={}"sv, order.client_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "&orderID={}"sv, order.external_order_id);
  }
  return buffer;
}

std::string_view Encoder::cancel_all_orders(
    std::string &buffer, CancelAllOrders const &, [[maybe_unused]] std::string_view const &request_id, std::string_view const &symbol) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "?symbol={}"sv, symbol);
  return buffer;
}

std::string_view Encoder::cancel_all_after(std::string &buffer, std::chrono::milliseconds timeout) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      R"({{)"
      R"("timeout":{})"
      R"(}})"sv,
      timeout.count());
  return buffer;
}

}  // namespace json
}  // namespace btse_futures
}  // namespace roq
