/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using namespace Catch::literals;

using value_type = protocol::json::Subscribe;

TEST_CASE("positions_v3", "[json_subscribe]") {
  auto message = R"({)"
                 R"("event":"subscribe",)"
                 R"("channel":["positionsV3"])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.event == protocol::json::Event::SUBSCRIBE);
    REQUIRE(std::size(obj.channel) == 1);
    CHECK(obj.channel[0] == "positionsV3"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("all_position_v4", "[json_subscribe]") {
  auto message = R"({)"
                 R"("event":"subscribe",)"
                 R"("channel":["allPositionV4"])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.event == protocol::json::Event::SUBSCRIBE);
    REQUIRE(std::size(obj.channel) == 1);
    CHECK(obj.channel[0] == "allPositionV4"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("notification_api_v4", "[json_subscribe]") {
  auto message = R"({)"
                 R"("event":"subscribe",)"
                 R"("channel":["notificationApiV4"])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.event == protocol::json::Event::SUBSCRIBE);
    REQUIRE(std::size(obj.channel) == 1);
    CHECK(obj.channel[0] == "notificationApiV4"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("fills_v2", "[json_subscribe]") {
  auto message = R"({)"
                 R"("event":"subscribe",)"
                 R"("channel":["fillsV2"])"
                 R"(})";
  auto helper = [](value_type const &obj) {
    CHECK(obj.event == protocol::json::Event::SUBSCRIBE);
    REQUIRE(std::size(obj.channel) == 1);
    CHECK(obj.channel[0] == "fillsV2"sv);
  };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
