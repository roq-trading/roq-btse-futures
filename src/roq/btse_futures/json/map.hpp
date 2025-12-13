/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/liquidity.hpp"
#include "roq/margin_mode.hpp"
#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/position_effect.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/update_type.hpp"

#include "roq/btse_futures/json/action.hpp"
#include "roq/btse_futures/json/asset_mode.hpp"
#include "roq/btse_futures/json/margin_mode.hpp"
#include "roq/btse_futures/json/order_status.hpp"
#include "roq/btse_futures/json/order_type.hpp"
#include "roq/btse_futures/json/pos_side.hpp"
#include "roq/btse_futures/json/side.hpp"
#include "roq/btse_futures/json/time_in_force.hpp"
#include "roq/btse_futures/json/trade_scope.hpp"
#include "roq/btse_futures/json/trade_side.hpp"
#include "roq/btse_futures/json/type.hpp"

namespace roq {

// btse_futures::json => roq

template <>
template <>
std::optional<UpdateType> Map<btse_futures::json::Action>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<btse_futures::json::AssetMode>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<btse_futures::json::MarginMode>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<btse_futures::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<btse_futures::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<btse_futures::json::PosSide, btse_futures::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<btse_futures::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<btse_futures::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<btse_futures::json::TradeScope>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<btse_futures::json::TradeSide>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<btse_futures::json::Type>::helper() const;

// roq => btse_futures::json

template <>
template <>
std::optional<btse_futures::json::MarginMode> Map<MarginMode>::helper() const;

template <>
template <>
std::optional<btse_futures::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<btse_futures::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<btse_futures::json::TradeSide> Map<PositionEffect>::helper() const;

template <>
template <>
std::optional<btse_futures::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<btse_futures::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
