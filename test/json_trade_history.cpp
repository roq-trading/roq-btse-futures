/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/trade_history.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

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
  core::json::BufferStack buffer{8192, 1};
  [[maybe_unused]] json::TradeHistory obj{message, buffer};
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
  core::json::BufferStack buffer{8192, 1};
  [[maybe_unused]] json::TradeHistory obj{message, buffer};
}
