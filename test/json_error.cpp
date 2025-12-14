/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/json/error.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = json::Error;

TEST_CASE("failure", "[json_error]") {
  auto message = R"({)"
                 R"("status":400,)"
                 R"("errorCode":10002,)"
                 R"("message":"User session is expired. Please login again",)"
                 R"("extraData":null)"
                 R"(})";
  auto helper = [&](value_type &obj) {
    CHECK(obj.status == 400);
    CHECK(obj.error_code == 10002);
    CHECK(obj.message == "User session is expired. Please login again"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("failure_2", "[json_error]") {
  auto message = R"({)"
                 R"("status":400,)"
                 R"("errorCode":-7,)"
                 R"("message":"Authenticate failed",)"
                 R"("extraData":null)"
                 R"(})";
  auto helper = [&](value_type &obj) {
    CHECK(obj.status == 400);
    CHECK(obj.error_code == -7);
    CHECK(obj.message == "Authenticate failed"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
