/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/btse_futures/protocol/json/error.hpp"

using namespace roq;
using namespace roq::btse_futures;

using namespace std::literals;

using value_type = protocol::json::Error;

TEST_CASE("user_session_expired", "[json_error]") {
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

TEST_CASE("authenticate_failed", "[json_error]") {
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

TEST_CASE("invalid_request_parameters", "[json_error]") {
  auto message = R"({)"
                 R"("status":400,)"
                 R"("errorCode":-2,)"
                 R"("message":"Invalid request parameters",)"
                 R"("extraData":null)"
                 R"(})";
  auto helper = [&](value_type &obj) {
    CHECK(obj.status == 400);
    CHECK(obj.error_code == -2);
    CHECK(obj.message == "Invalid request parameters"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("order_size_too_small", "[json_error]") {
  auto message = R"({)"
                 R"("status":400,)"
                 R"("errorCode":4050,)"
                 R"("message":"[BTC-PERP] The order size must be at least 1.0 contracts. Please adjust your order size and try again.",)"
                 R"("extraData":null)"
                 R"(})";
  auto helper = [&](value_type &obj) {
    CHECK(obj.status == 400);
    CHECK(obj.error_code == 4050);
    CHECK(obj.message == "[BTC-PERP] The order size must be at least 1.0 contracts. Please adjust your order size and try again."sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("unknown_cl_ord_id", "[json_error]") {
  auto message = R"({)"
                 R"("status":400,)"
                 R"("errorCode":-1,)"
                 R"("message":"clOrderID doesn't exist: XgACx2sZ6kUAAQAAAAAA",)"
                 R"("extraData":null)"
                 R"(})";
  auto helper = [&](value_type &obj) {
    CHECK(obj.status == 400);
    CHECK(obj.error_code == -1);
    CHECK(obj.message == "clOrderID doesn't exist: XgACx2sZ6kUAAQAAAAAA"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
