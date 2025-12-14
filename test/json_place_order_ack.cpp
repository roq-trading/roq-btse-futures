/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/place_order_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::PlaceOrderAck;

TEST_CASE("failure", "[json_place_order_ack]") {
  auto message =
      R"([{)"
      R"("status":8,)"
      R"("symbol":"BTC-PERP",)"
      R"("orderType":0,)"
      R"("price":0,)"
      R"("side":"SELL",)"
      R"("orderID":"37ed16b1-06da-46f5-8fc1-a81df56f9d08",)"
      R"("timestamp":1765714801428,)"
      R"("triggerPrice":0,)"
      R"("trigger":false,)"
      R"("deviation":100,)"
      R"("stealth":100,)"
      R"("message":"{\"msgKey\":\"trade.error.insufficient_balance1\", \"default_msg\":\"Insufficient available balance in cross wallet, minimum required is 0.013152\", \"params\":[\"cross\", 0.013152]}",)"
      R"("avgFilledPrice":0,)"
      R"("clOrderID":"VAACv2tp6EUAAQAAAAAA",)"
      R"("originalOrderSize":0,)"
      R"("currentOrderSize":0,)"
      R"("filledSize":0,)"
      R"("totalFilledSize":0,)"
      R"("remainingSize":0,)"
      R"("postOnly":false,)"
      R"("orderDetailType":null,)"
      R"("positionMode":"ONE_WAY",)"
      R"("positionDirection":null,)"
      R"("positionId":null,)"
      R"("time_in_force":"GTC")"
      R"(})"
      R"(])";
  auto helper = [&](value_type &obj) {
    REQUIRE(std::size(obj.data) == 1);
    auto &d0 = obj.data[0];
    CHECK(d0.status == 8);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("success", "[json_place_order_ack]") {
  auto message = R"([{)"
                 R"("status":2,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderType":76,)"
                 R"("price":32000,)"
                 R"("side":"BUY",)"
                 R"("orderID":"c9a173ab-0fc8-4bdc-b940-1c7d7fe9fb89",)"
                 R"("timestamp":1765718100570,)"
                 R"("triggerPrice":0,)"
                 R"("trigger":false,)"
                 R"("deviation":100,)"
                 R"("stealth":100,)"
                 R"("message":"",)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"DgAC_ntg6kUAAQAAAAAA",)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":0,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":1,)"
                 R"("postOnly":false,)"
                 R"("orderDetailType":null,)"
                 R"("positionMode":"ONE_WAY",)"
                 R"("positionDirection":null,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("time_in_force":"GTC")"
                 R"(})"
                 R"(])";
  auto helper = [&](value_type &obj) {
    REQUIRE(std::size(obj.data) == 1);
    auto &d0 = obj.data[0];
    CHECK(d0.status == 2);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
