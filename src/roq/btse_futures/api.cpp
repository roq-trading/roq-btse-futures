/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/api.hpp"

#include "roq/logging.hpp"

#include "roq/utils/enum.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === IMPLEMENTATION ===

API API::create(Settings const &) {
  return {
      .category = {},
      .inst_type = {},
      .market_data{
          .market_summary = "/api/v2.1/market_summary"sv,
      },
      .order_management{
          .account_info = "/api/v3/account/settings"sv,
          .account_assets = "/api/v3/account/assets"sv,
          .position_info = "/api/v3/position/current-position"sv,
          .open_orders = "/api/v3/trade/unfilled-orders"sv,
          .fill_history = "/api/v3/trade/fills"sv,
          .place_order = "/api/v3/trade/place-order"sv,
          .modify_order = "/api/v3/trade/modify-order"sv,
          .cancel_order = "/api/v3/trade/cancel-order"sv,
          .cancel_all_orders = "/api/v3/trade/cancel-symbol-order"sv,
          .countdown_cancel_all = "/api/v3/trade/countdown-cancel-all"sv,
      },
  };
}

}  // namespace btse_futures
}  // namespace roq
