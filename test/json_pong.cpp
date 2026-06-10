/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = protocol::json::Pong;

TEST_CASE("snapshot", "[json_pong]") {
  /*
  auto message = "pong"sv;
  auto helper = [](value_type const &) {};
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
  */
}
