/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/protocol/json/modify_order_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = protocol::json::ModifyOrderAck;

TEST_CASE("success", "[json_modify_order_ack]") {
  auto message = R"([{)"
                 R"("status":123,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderType":76,)"
                 R"("price":32323,)"
                 R"("side":"BUY",)"
                 R"("orderID":"1c2b0729-e661-4880-924a-01d5f2c522da",)"
                 R"("timestamp":1765720603681,)"
                 R"("triggerPrice":0,)"
                 R"("trigger":false,)"
                 R"("deviation":100,)"
                 R"("stealth":100,)"
                 R"("message":"",)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"rQACsyjc60UAAQAAAAAA",)"
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
    CHECK(d0.status == 123);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
