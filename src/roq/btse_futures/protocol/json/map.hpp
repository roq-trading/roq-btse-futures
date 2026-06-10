/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/update_type.hpp"

#include "roq/btse_futures/protocol/json/order_state.hpp"
#include "roq/btse_futures/protocol/json/order_type.hpp"
#include "roq/btse_futures/protocol/json/side.hpp"
#include "roq/btse_futures/protocol/json/time_in_force.hpp"
#include "roq/btse_futures/protocol/json/type.hpp"

namespace roq {

// btse_futures::json => roq

template <>
template <>
std::optional<OrderStatus> Map<int32_t>::helper() const;

template <>
template <>
std::optional<OrderType> Map<int32_t>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<btse_futures::protocol::json::OrderState>::helper() const;

template <>
template <>
std::optional<OrderType> Map<btse_futures::protocol::json::OrderType>::helper() const;

template <>
template <>
std::optional<Side> Map<btse_futures::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<btse_futures::protocol::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<btse_futures::protocol::json::Type>::helper() const;

// roq => btse_futures::json

template <>
template <>
std::optional<btse_futures::protocol::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<btse_futures::protocol::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<btse_futures::protocol::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
