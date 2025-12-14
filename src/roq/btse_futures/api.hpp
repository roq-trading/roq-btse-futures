/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/btse_futures/settings.hpp"

namespace roq {
namespace btse_futures {

struct API final {
  struct {
    std::string_view market_summary;
  } market_data;

  struct {
    std::string_view position_mode;
    std::string_view wallet;
    std::string_view unified_wallet;
    std::string_view wallet_margin;
    std::string_view positions;
    std::string_view open_orders;
    std::string_view trade_history;
    std::string_view create_order;
    std::string_view amend_order;
    std::string_view cancel_order;
    std::string_view cancel_all_after;
  } order_management;

  // factory
  static API create(Settings const &);
};

}  // namespace btse_futures
}  // namespace roq
