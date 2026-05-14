/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <cstdint>

namespace roq {
namespace btse_futures {

enum class OrderEntryState : uint8_t {
  UNDEFINED = 0,
  POSITION_MODE,
  WALLET,
  POSITIONS,
  OPEN_ORDERS,
  FILL_HISTORY,
  DONE,
};

}  // namespace btse_futures
}  // namespace roq
