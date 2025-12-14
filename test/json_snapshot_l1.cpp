/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = json::SnapshotL1;

TEST_CASE("simple", "[json_snapshot_l1]") {
  auto message = R"({)"
                 R"("topic":"snapshotL1:ETHPFC",)"
                 R"("data":{)"
                 R"("bids":[["4386.37","8403"]],)"
                 R"("asks":[["4386.58","8860"]],)"
                 R"("type":"snapshotL1",)"
                 R"("symbol":"ETHPFC",)"
                 R"("timestamp":1759373840687)"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.topic == "snapshotL1:ETHPFC"sv);
    REQUIRE(std::size(obj.data.bids) == 1);
    REQUIRE(std::size(obj.data.asks) == 1);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
