/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex/json/market24h.hpp"

using namespace roq;
using namespace roq::phemex;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

TEST_CASE("simple", "[json_market24h]") {
  auto message = R"({)"
                 R"("market24h":{)"
                 R"("close":1094389000,)"
                 R"("fundingRate":2481,)"
                 R"("high":1097721000,)"
                 R"("indexPrice":1095022905,)"
                 R"("low":1090714000,)"
                 R"("markPrice":1094389000,)"
                 R"("open":1094937000,)"
                 R"("openInterest":2521160395,)"
                 R"("predFundingRate":2481,)"
                 R"("symbol":"BTCUSD",)"
                 R"("turnover":118157120811,)"
                 R"("volume":129318979)"
                 R"(},)"
                 R"("timestamp":1759029819812080202)"
                 R"(})";
  core::json::BufferStack buffer{8192, 1};
  json::Market24h obj{message, buffer};
}
