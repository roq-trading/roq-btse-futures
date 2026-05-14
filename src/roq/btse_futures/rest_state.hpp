/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <cstdint>

namespace roq {
namespace btse_futures {

enum class RestState : uint8_t {
  UNDEFINED = 0,
  MARKET_SUMMARY,
  DONE,
};

}  // namespace btse_futures
}  // namespace roq
