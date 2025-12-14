/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = json::Notification;

TEST_CASE("simple", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"1a2e349b-cad4-4547-b574-389350e83f96",)"
                 R"("side":"",)"
                 R"("orderType":0,)"
                 R"("type":0,)"
                 R"("price":0,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":8,)"
                 R"("timestamp":1765712641867,)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"0AACCB4g50UAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":false,)"
                 R"("positionId":null,)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":false,)"
                 R"("originalOrderSize":0,)"
                 R"("currentOrderSize":0,)"
                 R"("filledSize":0,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":0,)"
                 R"("time_in_force":"GTC")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "notificationApiV4"sv);
    REQUIRE(std::size(obj.data) == 1);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
