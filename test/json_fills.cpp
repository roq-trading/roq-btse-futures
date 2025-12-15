/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = json::Fills;

TEST_CASE("buy", "[json_fills]") {
  auto message = R"({)"
                 R"("topic":"fillsV2",)"
                 R"("data":[{)"
                 R"("orderId":"aa0eb277-2d36-4fd0-8fa0-bc78b139ccdc",)"
                 R"("serialId":937242896,)"
                 R"("clOrderId":"LgACO7WlB0YAAQAAAAAA",)"
                 R"("type":"76",)"
                 R"("symbol":"BTC-PERP",)"
                 R"("side":"BUY",)"
                 R"("price":"89315.3",)"
                 R"("size":"1.0",)"
                 R"("feeAmount":"0.00049123",)"
                 R"("feeCurrency":"USDT",)"
                 R"("base":"BTC-PERP",)"
                 R"("quote":"USDT",)"
                 R"("maker":false,)"
                 R"("contractSize":0.00001,)"
                 R"("timestamp":1765767211936,)"
                 R"("tradeId":"128c163c-1267-422a-9082-029472e16a36")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "fillsV2"sv);
    REQUIRE(std::size(obj.data) == 1);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("sell", "[json_fills]") {
  auto message = R"({)"
                 R"("topic":"fillsV2",)"
                 R"("data":[{)"
                 R"("orderId":"73649e44-360a-4287-8952-3dd5da2f2e4b",)"
                 R"("serialId":937305910,)"
                 R"("clOrderId":"MQACLRoOCkYAAQAAAAAA",)"
                 R"("type":"76",)"
                 R"("symbol":"BTC-PERP",)"
                 R"("side":"SELL",)"
                 R"("price":"89289.7",)"
                 R"("size":"1.0",)"
                 R"("feeAmount":"0.00049109",)"
                 R"("feeCurrency":"USDT",)"
                 R"("base":"BTC-PERP",)"
                 R"("quote":"USDT",)"
                 R"("maker":false,)"
                 R"("contractSize":0.00001,)"
                 R"("timestamp":1765771269202,)"
                 R"("tradeId":"0825635e-632b-4c08-adaf-466805512a22")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "fillsV2"sv);
    REQUIRE(std::size(obj.data) == 1);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("maker", "[json_fills]") {
  auto message = R"({)"
                 R"("topic":"fillsV2",)"
                 R"("data":[{)"
                 R"("orderId":"e9013cba-e46e-4d00-8503-b8851ec35339",)"
                 R"("serialId":937352783,)"
                 R"("clOrderId":"6wACj6XIC0YAAQAAAAAA",)"
                 R"("type":"76",)"
                 R"("symbol":"BTC-PERP",)"
                 R"("side":"BUY",)"
                 R"("price":"89760.0",)"
                 R"("size":"1.0",)"
                 R"("feeAmount":"0.00049368",)"
                 R"("feeCurrency":"USDT",)"
                 R"("base":"BTC-PERP",)"
                 R"("quote":"USDT",)"
                 R"("maker":false,)"  // XXX FIXME TODO ???
                 R"("contractSize":0.00001,)"
                 R"("timestamp":1765774482920,)"
                 R"("tradeId":"8096588d-2691-4115-981a-9fd6b2981f7e")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "fillsV2"sv);
    REQUIRE(std::size(obj.data) == 1);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
