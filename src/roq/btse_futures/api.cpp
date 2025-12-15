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
      .market_data{
          .market_summary = "/api/v2.3/market_summary"sv,
      },
      .order_management{
          .position_mode = "/api/v2.3/position_mode"sv,
          .wallet = "/api/v2.3/user/wallet"sv,
          .unified_wallet = "/api/v2.3/user/unifiedWallet/margin"sv,
          .wallet_margin = "/api/v2.3/user/margin"sv,
          .positions = "/api/v2.3/user/positions"sv,
          .open_orders = "/api/v2.3/user/open_orders"sv,
          .trade_history = "/api/v2.3/user/trade_history"sv,
          .create_order = "/api/v2.3/order"sv,
          .amend_order = "/api/v2.3/order"sv,
          .cancel_order = "/api/v2.3/order"sv,
          .cancel_all_orders = "/api/v2.3/order"sv,
          .cancel_all_after = "/api/v2.3/order/cancelAllAfter"sv,
      },
  };
}

}  // namespace btse_futures
}  // namespace roq
