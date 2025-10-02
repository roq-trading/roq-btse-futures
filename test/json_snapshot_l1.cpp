/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/snapshot_l1.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

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
  core::json::BufferStack buffer_stack{8192, 2};
  json::SnapshotL1 obj{message, buffer_stack};
}
