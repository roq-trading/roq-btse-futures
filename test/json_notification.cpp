/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = json::Notification;

TEST_CASE("rejected", "[json_notification]") {
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

TEST_CASE("created", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"c9a173ab-0fc8-4bdc-b940-1c7d7fe9fb89",)"
                 R"("side":"BUY",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":32000,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":2,)"
                 R"("timestamp":1765718100570,)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"DgAC_ntg6kUAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":true,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":true,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":0,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":1,)"
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

TEST_CASE("canceled", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"6f47868c-1823-4d19-8cd9-9b987d12cc23",)"
                 R"("side":"BUY",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":32000,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":6,)"
                 R"("timestamp":1765719967163,)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"5AACLlx860UAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":true,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":true,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":0,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":1,)"
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

TEST_CASE("modified", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"1c2b0729-e661-4880-924a-01d5f2c522da",)"
                 R"("side":"BUY",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":32323,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":123,)"
                 R"("timestamp":1765720603681,)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"rQACsyjc60UAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":true,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":true,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":0,)"
                 R"("totalFilledSize":0,)"
                 R"("remainingSize":1,)"
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
