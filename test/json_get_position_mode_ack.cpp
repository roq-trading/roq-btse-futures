/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/protocol/json/get_position_mode_ack.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = protocol::json::GetPositionModeAck;

TEST_CASE("empty", "[json_get_position_mode_ack]") {
  auto message = R"([)"
                 R"({"symbol":"TIA-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"SUI-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"JUP-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"HYPE-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"PENDLE-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"FET-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"C98-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"ENA-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"ATOM-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"TRUMP-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"BOME-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"1KPEPE-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"XRP-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"W-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"1KFLOKI-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"1KSHIB-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"LINK-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"IOTX-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"PYTH-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"NEO-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"WIF-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"TOKEN-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"DOT-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"ETH-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"TRB-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"LTC-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"BNB-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"BCH-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"KAS-PERP","positionMode":"ONE_WAY"},)"
                 R"({"symbol":"BTC-PERP","positionMode":"ONE_WAY"})"
                 R"(])";
  auto helper = [&](value_type &obj) {
    REQUIRE(std::size(obj.data) == 30);
    auto &d0 = obj.data[0];
    CHECK(d0.symbol == "TIA-PERP"sv);
    auto &d29 = obj.data[29];
    CHECK(d29.symbol == "BTC-PERP"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
