/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/phemex/json/trades.hpp"

using namespace roq;
using namespace roq::phemex;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_trades]") {
  auto message = R"({)"
                 R"("sequence":20254182892,)"
                 R"("symbol":"BTCUSD",)"
                 R"("trades":[)"
                 R"([1758962421019961472,"Sell",1093078000,1],)"
                 R"([1758962421019961472,"Sell",1093110000,1],)"
                 R"([1758950929672637218,"Buy",1095929000,201],)"
                 R"([1758950929672637218,"Buy",1095929000,201])"
                 R"(],)"
                 R"("type":"snapshot")"
                 R"(})";
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::Trades obj{message, buffer};
}
