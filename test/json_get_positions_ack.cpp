/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/get_positions_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = json::GetPositionsAck;

TEST_CASE("success", "[json_get_positions_ack]") {
  auto message = R"([)"
                 R"(])";
  auto helper = [&](value_type &obj) { REQUIRE(std::empty(obj.data)); };
  core::json::BufferStack buffers{8192, 2};
  value_type obj{message, buffers};
  helper(obj);
}
