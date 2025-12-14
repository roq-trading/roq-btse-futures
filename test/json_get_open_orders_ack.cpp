/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/get_open_orders_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::GetOpenOrdersAck;

TEST_CASE("empty", "[json_get_open_orders_ack]") {
  auto message = R"([)"
                 R"(])";
  auto helper = [&](value_type &obj) { REQUIRE(std::empty(obj.data)); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("simple", "[json_get_open_orders_ack]") {
  auto message = R"([{)"
                 R"("vendorName":null,)"
                 R"("botID":null,)"
                 R"("orderType":76,)"
                 R"("price":32000,)"
                 R"("side":"BUY",)"
                 R"("orderValue":0.32,)"
                 R"("pegPriceMin":0,)"
                 R"("pegPriceMax":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("cancelDuration":0,)"
                 R"("timestamp":1765718100570,)"
                 R"("orderID":"c9a173ab-0fc8-4bdc-b940-1c7d7fe9fb89",)"
                 R"("stealth":1,)"
                 R"("triggerOrder":false,)"
                 R"("triggered":false,)"
                 R"("triggerPrice":0,)"
                 R"("triggerOriginalPrice":0,)"
                 R"("triggerOrderType":0,)"
                 R"("triggerTrailingStopDeviation":0,)"
                 R"("triggerStopPrice":0,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("trailValue":0,)"
                 R"("trailValueType":"DISTANCE",)"
                 R"("contractSize":0.00001,)"
                 R"("activationPrice":null,)"
                 R"("activationPriceType":null,)"
                 R"("ocoPeerOrderId":null,)"
                 R"("currentPegPrice":32000,)"
                 R"("clOrderID":"DgAC_ntg6kUAAQAAAAAA",)"
                 R"("reduceOnly":false,)"
                 R"("orderState":"STATUS_ACTIVE",)"
                 R"("triggerUseLastPrice":false,)"
                 R"("avgFilledPrice":0,)"
                 R"("timeInForce":"GTC",)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":1,)"
                 R"("orderDetailType":null,)"
                 R"("takeProfitOrder":null,)"
                 R"("stopLossOrder":null,)"
                 R"("closeOrder":false,)"
                 R"("positionMode":"ONE_WAY",)"
                 R"("positionDirection":null,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("wrapperOrder":false)"
                 R"(})"
                 R"(])";
  auto helper = [&](value_type &obj) {
    REQUIRE(std::size(obj.data) == 1);
    auto &d0 = obj.data[0];
    CHECK(d0.order_type == 76);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
