/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/update_type.hpp"

#include "roq/btse_futures/json/order_type.hpp"
#include "roq/btse_futures/json/side.hpp"
#include "roq/btse_futures/json/time_in_force.hpp"
#include "roq/btse_futures/json/type.hpp"

namespace roq {

// btse_futures::json => roq

template <>
template <>
std::optional<OrderStatus> Map<int32_t>::helper() const;

template <>
template <>
std::optional<OrderType> Map<btse_futures::json::OrderType>::helper() const;

template <>
template <>
std::optional<Side> Map<btse_futures::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<btse_futures::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<btse_futures::json::Type>::helper() const;

// roq => btse_futures::json

template <>
template <>
std::optional<btse_futures::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<btse_futures::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<btse_futures::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
