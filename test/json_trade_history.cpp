/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::TradeHistory;

// note! truncated
TEST_CASE("snapshot", "[json_trade_history]") {
  auto message = R"({)"
                 R"("topic":"tradeHistoryApi",)"
                 R"("data":[{)"
                 R"("price":117657.3,)"
                 R"("size":13,)"
                 R"("side":"BUY",)"
                 R"("symbol":"BTCPFC",)"
                 R"("tradeId":929934857,)"
                 R"("timestamp":1759325682002)"
                 R"(},{)"
                 R"("price":117652.7,)"
                 R"("size":13,)"
                 R"("side":"SELL",)"
                 R"("symbol":"BTCPFC",)"
                 R"("tradeId":929934855,)"
                 R"("timestamp":1759325681975)"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "tradeHistoryApi"sv);
    REQUIRE(std::size(obj.data) == 2);
    CHECK(obj.data[0].price == 117657.3_a);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

// note! can't really tell the difference...
TEST_CASE("incremental", "[json_trade_history]") {
  auto message = R"({)"
                 R"("topic":"tradeHistoryApi",)"
                 R"("data":[{)"
                 R"("price":117661.3,)"
                 R"("size":13,)"
                 R"("side":"BUY",)"
                 R"("symbol":"BTCPFC",)"
                 R"("tradeId":929934907,)"
                 R"("timestamp":1759326050158)"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "tradeHistoryApi"sv);
    REQUIRE(std::size(obj.data) == 1);
    CHECK(obj.data[0].price == 117661.3_a);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
