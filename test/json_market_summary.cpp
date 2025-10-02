/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/market_summary.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

// note! reduced
TEST_CASE("simple", "[json_market_summary]") {
  auto message = R"([{)"
                 R"("symbol":"TIAPFC",)"
                 R"("last":0.001,)"
                 R"("lowestAsk":3.768,)"
                 R"("highestBid":0.001,)"
                 R"("openInterest":59549,)"
                 R"("openInterestUSD":8149.28,)"
                 R"("percentageChange":0,)"
                 R"("volume":0.0221,)"
                 R"("high24Hr":0.001,)"
                 R"("low24Hr":0.001,)"
                 R"("base":"TIA",)"
                 R"("quote":"USD",)"
                 R"("contractStart":0,)"
                 R"("contractEnd":0,)"
                 R"("active":true,)"
                 R"("timeBasedContract":false,)"
                 R"("openTime":0,)"
                 R"("closeTime":0,)"
                 R"("startMatching":0,)"
                 R"("inactiveTime":0,)"
                 R"("fundingRate":0.0000984,)"
                 R"("contractSize":0.1,)"
                 R"("maxPosition":1500000,)"
                 R"("minValidPrice":0.001,)"
                 R"("minPriceIncrement":0.001,)"
                 R"("minOrderSize":1,)"
                 R"("maxOrderSize":200000,)"
                 R"("minRiskLimit":150000,)"
                 R"("maxRiskLimit":1500000,)"
                 R"("minSizeIncrement":1,)"
                 R"("availableSettlement":["USD","USDT","USDC","BTC","ETH","AED","AUD","CAD","CHF","EUR","GBP","HKD","INR","JPY","MYR","NZD","SGD"])"
                 R"(},{)"
                 R"("symbol":"SUIPFC",)"
                 R"("last":3.0096,)"
                 R"("lowestAsk":2.846,)"
                 R"("highestBid":2.8391,)"
                 R"("openInterest":116004,)"
                 R"("openInterestUSD":376735.41,)"
                 R"("percentageChange":0,)"
                 R"("volume":0,)"
                 R"("high24Hr":3.0096,)"
                 R"("low24Hr":3.0096,)"
                 R"("base":"SUI",)"
                 R"("quote":"USD",)"
                 R"("contractStart":0,)"
                 R"("contractEnd":0,)"
                 R"("active":true,)"
                 R"("timeBasedContract":false,)"
                 R"("openTime":0,)"
                 R"("closeTime":0,)"
                 R"("startMatching":0,)"
                 R"("inactiveTime":0,)"
                 R"("fundingRate":0.00010056,)"
                 R"("contractSize":1,)"
                 R"("maxPosition":1800000,)"
                 R"("minValidPrice":0.0001,)"
                 R"("minPriceIncrement":0.0001,)"
                 R"("minOrderSize":1,)"
                 R"("maxOrderSize":150000,)"
                 R"("minRiskLimit":180000,)"
                 R"("maxRiskLimit":1800000,)"
                 R"("minSizeIncrement":1,)"
                 R"("availableSettlement":["USD","USDT","USDC","BTC","ETH","AED","AUD","CAD","CHF","EUR","GBP","HKD","INR","JPY","MYR","NZD","SGD"])"
                 R"(})"
                 R"(])";
  core::json::BufferStack buffer{65536, 1};
  [[maybe_unused]] json::MarketSummary obj{message, buffer};
}
