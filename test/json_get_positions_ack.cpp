/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/protocol/json/get_positions_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = protocol::json::GetPositionsAck;

TEST_CASE("flat", "[json_get_positions_ack]") {
  auto message = R"([)"
                 R"(])";
  auto helper = [&](value_type &obj) { REQUIRE(std::empty(obj.data)); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("long", "[json_get_positions_ack]") {
  auto message = R"([{)"
                 R"("marginType":91,)"
                 R"("entryPrice":89315.3,)"
                 R"("markPrice":89244.40954762,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("side":"BUY",)"
                 R"("orderValue":0.8924441,)"
                 R"("settleWithAsset":"USDT",)"
                 R"("unrealizedProfitLoss":-0.0007089,)"
                 R"("totalMaintenanceMargin":0.00495307,)"
                 R"("size":1,)"
                 R"("liquidationPrice":0,)"
                 R"("isolatedLeverage":25,)"
                 R"("adlScoreBucket":1,)"
                 R"("contractSize":0.00001,)"
                 R"("liquidationInProgress":false,)"
                 R"("timestamp":1765770265003,)"
                 R"("takeProfitOrder":null,)"
                 R"("stopLossOrder":null,)"
                 R"("positionMode":"ONE_WAY",)"
                 R"("positionDirection":null,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("walletName":"CROSS@",)"
                 R"("currentLeverage":0,)"
                 R"("minimumRequiredMargin":0)"
                 R"(})"
                 R"(])";
  auto helper = [&](value_type &obj) { REQUIRE(std::size(obj.data) == 1); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("short", "[json_get_positions_ack]") {
  auto message = R"([{)"
                 R"("marginType":91,)"
                 R"("entryPrice":89502.7,)"
                 R"("markPrice":89498.72947576,)"
                 R"("symbol":"BTC-PERP",)"
                 R"("side":"SELL",)"
                 R"("orderValue":0.89498729,)"
                 R"("settleWithAsset":"USDT",)"
                 R"("unrealizedProfitLoss":0.00003971,)"
                 R"("totalMaintenanceMargin":0.00498179,)"
                 R"("size":1,)"
                 R"("liquidationPrice":994520017.5432578,)"
                 R"("isolatedLeverage":25,)"
                 R"("adlScoreBucket":2,)"
                 R"("contractSize":0.00001,)"
                 R"("liquidationInProgress":false,)"
                 R"("timestamp":1765773272571,)"
                 R"("takeProfitOrder":null,)"
                 R"("stopLossOrder":null,)"
                 R"("positionMode":"ONE_WAY",)"
                 R"("positionDirection":null,)"
                 R"("positionId":"BTC-PERP-USDT",)"
                 R"("walletName":"CROSS@",)"
                 R"("currentLeverage":0,)"
                 R"("minimumRequiredMargin":0)"
                 R"(})"
                 R"(])";
  auto helper = [&](value_type &obj) { REQUIRE(std::size(obj.data) == 1); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
