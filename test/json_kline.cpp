/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex/json/kline.hpp"

using namespace roq;
using namespace roq::phemex;

using namespace std::literals;

// note! truncated
TEST_CASE("snapshot", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040340,60,1093523000,1093522000,1093523000,1093522000,1093522000,94584,86494822],)"
                 R"([1759040280,60,1093522000,1093523000,1093523000,1093523000,1093523000,248,226789],)"
                 R"([1758920460,60,1092502000,1092503000,1092503000,1092503000,1092503000,1,915],)"
                 R"([1758920400,60,1093174000,1093111000,1093111000,1092502000,1092502000,106,96972])"
                 R"(],)"
                 R"("priceScale":4,)"
                 R"("sequence":20256660818,)"
                 R"("symbol":"BTCUSD",)"
                 R"("type":"snapshot",)"
                 R"("valueScale":8)"
                 R"(})"
                 R"(})";
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::Kline obj{message, buffer};
}

TEST_CASE("incremental", "[json_kline]") {
  auto message = R"({)"
                 R"("kline":[)"
                 R"([1759040820,60,6459000,6468000,6483000,6468000,6483000,269388700000,17438876605])"
                 R"(],)"
                 R"("priceScale":8,)"
                 R"("qtyScale":8,)"
                 R"("sequence":18268599368,)"
                 R"("symbol":"sPUMPBTCUSDT",)"
                 R"("type":"incremental",)"
                 R"("valueScale":8)"
                 R"(})";
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::Kline obj{message, buffer};
}
