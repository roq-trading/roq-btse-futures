/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::Update;

// note! truncated
TEST_CASE("snapshot", "[json_update]") {
  auto message = R"({)"
                 R"("topic":"update:ETHPFC_0",)"
                 R"("data":{)"
                 R"("bids":[)"
                 R"(["4390.97","19655"],)"
                 R"(["4390.74","6548"],)"
                 R"(["4379.93","19909"],)"
                 R"(["4379.70","16826"])"
                 R"(],)"
                 R"("asks":[)"
                 R"(["4402.70","20849"],)"
                 R"(["4402.47","3554"],)"
                 R"(["4391.66","12443"],)"
                 R"(["4391.43","11514"])"
                 R"(],)"
                 R"("seqNum":8935263,)"
                 R"("prevSeqNum":8935262,)"
                 R"("type":"snapshot",)"
                 R"("symbol":"ETHPFC",)"
                 R"("timestamp":1759375303714)"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "update:ETHPFC_0"sv);
    REQUIRE(std::size(obj.data.bids) == 4);
    REQUIRE(std::size(obj.data.asks) == 4);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("delta", "[json_update]") {
  auto message = R"({)"
                 R"("topic":"update:BTCPFC_0",)"
                 R"("data":{)"
                 R"("bids":[)"
                 R"(["119004.1","28425"],)"
                 R"(["118948.9","17599"],)"
                 R"(["118939.7","26484"])"
                 R"(],)"
                 R"("asks":[)"
                 R"(["119105.3","20490"],)"
                 R"(["119089.2","28052"],)"
                 R"(["119070.8","8945"],)"
                 R"(["119038.6","12607"])"
                 R"(],)"
                 R"("seqNum":9814889,)"
                 R"("prevSeqNum":9814888,)"
                 R"("type":"delta",)"
                 R"("symbol":"BTCPFC",)"
                 R"("timestamp":1759376047437)"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "update:BTCPFC_0"sv);
    REQUIRE(std::size(obj.data.bids) == 3);
    REQUIRE(std::size(obj.data.asks) == 4);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
