/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/cancel_order_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::CancelOrderAck;

TEST_CASE("success", "[json_cancel_order_ack]") {
  auto message = R"([{)"
                 R"("status":6,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderType":76,)"
                 R"("price":32000,)"
                 R"("side":"BUY",)"
                 R"("orderID":"6f47868c-1823-4d19-8cd9-9b987d12cc23",)"
                 R"("timestamp":1765719967163,)"
                 R"("triggerPrice":0,)"
                 R"("trigger":false,)"
                 R"("deviation":100,)"
                 R"("stealth":100,)"
                 R"("message":"",)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"5AACLlx860UAAQAAAAAA",)"
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
    CHECK(d0.status == 6);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
