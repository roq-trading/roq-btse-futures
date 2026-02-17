/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

#include "roq/btse_futures/json/map.hpp"

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

TEST_CASE("buy_filled", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"aa0eb277-2d36-4fd0-8fa0-bc78b139ccdc",)"
                 R"("side":"BUY",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":89315.3,)"  // note! the limit-price was 89400, this is last-traded-price
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":4,)"
                 R"("timestamp":1765767211936,)"
                 R"("avgFilledPrice":89315.3,)"
                 R"("clOrderID":"LgACO7WlB0YAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":false,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":true,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":1,)"
                 R"("totalFilledSize":1,)"
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

TEST_CASE("sell_filled", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"73649e44-360a-4287-8952-3dd5da2f2e4b",)"
                 R"("side":"SELL",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":89289.7,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":4,)"
                 R"("timestamp":1765771269202,)"
                 R"("avgFilledPrice":89289.7,)"
                 R"("clOrderID":"MQACLRoOCkYAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":false,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":true,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":1,)"
                 R"("totalFilledSize":1,)"
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

TEST_CASE("filled_maker", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"BTC-PERP",)"
                 R"("orderID":"e9013cba-e46e-4d00-8503-b8851ec35339",)"
                 R"("side":"BUY",)"
                 R"("orderType":76,)"
                 R"("type":0,)"
                 R"("price":89760,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":4,)"
                 R"("timestamp":1765774482920,)"
                 R"("avgFilledPrice":89760,)"
                 R"("clOrderID":"6wACj6XIC0YAAQAAAAAA",)"
                 R"("postOnly":false,)"
                 R"("maker":false,)"  // XXX FIXME TODO ???
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("orderDetailType":null,)"
                 R"("orderUserInitiated":false,)"
                 R"("originalOrderSize":1,)"
                 R"("currentOrderSize":1,)"
                 R"("filledSize":1,)"
                 R"("totalFilledSize":1,)"
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

TEST_CASE("issue_20260205", "[json_notification]") {
  auto message = R"({)"
                 R"("topic":"notificationApiV4",)"
                 R"("data":[{)"
                 R"("symbol":"SOL-PERP",)"
                 R"("orderID":"9599e536-252c-4e40-97c8-320440682bae",)"
                 R"("side":"",)"
                 R"("orderType":0,)"
                 R"("type":0,)"
                 R"("price":0,)"
                 R"("triggerPrice":0,)"
                 R"("pegPriceDeviation":1,)"
                 R"("stealth":1,)"
                 R"("status":8,)"
                 R"("timestamp":1770329155384,)"
                 R"("avgFilledPrice":0,)"
                 R"("clOrderID":"ogABDRAOplAAAQAAAAAA",)"
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
    auto &d0 = obj.data[0];
    // HERE
    auto order_type = map(d0.order_type).template get<OrderType>();
    CHECK(order_type == OrderType::UNDEFINED);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
