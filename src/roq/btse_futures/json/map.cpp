/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// btse_futures::json => roq

// int32_t => roq::OrderStatus

template <>
template <>
constexpr Helper<int32_t>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    case 0:  // UNDOC
      return roq::OrderStatus::UNDEFINED;
    case 1:  //  MARKET_UNAVAILABLE = Futures market is unavailable
      return roq::OrderStatus::UNDEFINED;
    case 2:  //  ORDER_INSERTED = Order is inserted successfully
      return roq::OrderStatus::WORKING;
    case 4:  //  ORDER_FULLY_TRANSACTED = Order is fully transacted
      return roq::OrderStatus::COMPLETED;
    case 5:  //  ORDER_PARTIALLY_TRANSACTED = Order is partially transacted
      return roq::OrderStatus::WORKING;
    case 6:  //  ORDER_CANCELLED = Order is cancelled successfully
      return roq::OrderStatus::CANCELED;
    case 7:  //  ORDER_REFUNDED = Order is refunded
      return roq::OrderStatus::UNDEFINED;
    case 8:  //  INSUFFICIENT_BALANCE = Insufficient balance in account
      return roq::OrderStatus::REJECTED;
    case 9:  //  TRIGGER_INSERTED = Trigger Order is inserted successfully
      return roq::OrderStatus::WORKING;
    case 10:  //  TRIGGER_ACTIVATED = Trigger Order is activated successfully
      return roq::OrderStatus::WORKING;
    case 11:  //  ERROR_INVALID_CURRENCY
      return roq::OrderStatus::REJECTED;
    case 12:  //  ERROR_UPDATE_RISK_LIMIT = Error in updating risk limit
      return roq::OrderStatus::REJECTED;
    case 13:  //  ERROR_INVALID_LEVERAGE
      return roq::OrderStatus::REJECTED;
    case 15:  //  ORDER_REJECTED = Order is rejected
      return roq::OrderStatus::REJECTED;
    case 16:  //  ORDER_NOTFOUND = Order is not found with the order ID or clOrderID provided
      return roq::OrderStatus::REJECTED;
    case 17:  //  REQUEST_FAILED = Failed to complete the request, please check order status
      return roq::OrderStatus::REJECTED;
    case 20:                               //  SUCCESS = Action succeeded.
      return roq::OrderStatus::UNDEFINED;  // ???
    case 21:                               //  FREEZE_SUCCESSFUL
      return roq::OrderStatus::UNDEFINED;  // ???
    case 27:                               //  TRANSFER_SUCCESSFUL = Transfer funds between futures and spot is successful
      return roq::OrderStatus::UNDEFINED;
    case 28:  //  TRANSFER_UNSUCCESSFUL = Transfer funds between spot and futures is unsuccessful
      return roq::OrderStatus::UNDEFINED;
    case 29:  //  QUERY_GET_ORDERS
      return roq::OrderStatus::UNDEFINED;
    case 31:  //  QUERY_GET_POSITIONS
      return roq::OrderStatus::UNDEFINED;
    case 33:  //  QUERY_GET_ALL_POSITIONS_ORDERS
      return roq::OrderStatus::UNDEFINED;
    case 34:  //  QUERY_WALLET
      return roq::OrderStatus::UNDEFINED;
    case 36:  //  QUERY_FUTURES_MARGIN
      return roq::OrderStatus::UNDEFINED;
    case 41:  //  ERROR_INVALID_RISK_LIMIT = Invalid risk limit was specified
      return roq::OrderStatus::REJECTED;
    case 51:  //  QUERY_GET_ORDERS_WITH_LIMIT
      return roq::OrderStatus::UNDEFINED;
    case 64:  //  STATUS_LIQUIDATION = Account is undergoing liquidation
      return roq::OrderStatus::REJECTED;
    case 65:                               //  STATUS_ACTIVE = Order is active
      return roq::OrderStatus::WORKING;    // ???
    case 66:                               //  MODE_BUY
      return roq::OrderStatus::UNDEFINED;  // ???
    case 76:                               //  ORDER_TYPE_LIMIT = Limit order
      return roq::OrderStatus::UNDEFINED;
    case 77:  //  ORDER_TYPE_MARKET = Market order
      return roq::OrderStatus::UNDEFINED;
    case 80:  //  ORDER_TYPE_PEG = Peg/Algo order
      return roq::OrderStatus::UNDEFINED;
    case 81:  //  ORDER_TYPE_OTC = Otc order
      return roq::OrderStatus::UNDEFINED;
    case 83:  //  MODE_SELL
      return roq::OrderStatus::UNDEFINED;
    case 85:  //  STATUS_PROCESSING = Order is inactive
      return roq::OrderStatus::UNDEFINED;
    case 88:  //  STATUS_INACTIVE = Order is inactive
      return roq::OrderStatus::UNDEFINED;
    case 101:  //  FUTURES_ORDER_PRICE_OUTSIDE_LIQUIDATION_PRICE = Futures order is outside of liquidation price
      return roq::OrderStatus::UNDEFINED;
    case 110:  //  FUTURES_FUNDING
      return roq::OrderStatus::UNDEFINED;
    case 123:  //  AMEND_ORDER = Order amended
      return roq::OrderStatus::UNDEFINED;
    case 124:  //  UNFREEZE_SUCCESSFUL
      return roq::OrderStatus::UNDEFINED;
    case 129:  //  FUTURES_CONFIG_MODE_CHANGE
      return roq::OrderStatus::UNDEFINED;
    case 131:  //  FUTURES_STATUS_PROCESSING_LEVERAGE
      return roq::OrderStatus::UNDEFINED;
    case 132:  //  FUTURES_STATUS_PROCESSING_RISK_LIMIT
      return roq::OrderStatus::UNDEFINED;
    case 133:  //  FUTURES_POSITION_MODE_INVALID
      return roq::OrderStatus::REJECTED;
    case 134:  //  POSITION_MODE_UNCHANGEABLE
      return roq::OrderStatus::UNDEFINED;
    case 138:  //  POSITION_MODE_CHANGE_PROCESSING
      return roq::OrderStatus::UNDEFINED;
    case 300:  //  ERROR_MAX_ORDER_SIZE_EXCEEDED
      return roq::OrderStatus::REJECTED;
    case 301:  //  ERROR_INVALID_ORDER_SIZE
      return roq::OrderStatus::REJECTED;
    case 302:  //  ERROR_INVALID_ORDER_PRICE
      return roq::OrderStatus::REJECTED;
    case 303:  //  ERROR_RATE_LIMITS_EXCEEDED
      return roq::OrderStatus::REJECTED;
    case 304:  //  ERROR_MAX_OPEN_ORDER_EXCEEDED
      return roq::OrderStatus::REJECTED;
    case 305:  //  ERROR_ORDER_PRICE_OUT_OF_PRICE_PROTECTION_RANGE
      return roq::OrderStatus::REJECTED;
    case 1003:  //  ORDER_LIQUIDATION = Order is undergoing liquidation
      return roq::OrderStatus::REJECTED;
    case 1004:  //  ORDER_ADL = Order is undergoing ADL
      return roq::OrderStatus::REJECTED;
    case 30410:  //  BLOCK_TRADE_COMPLETE_SUCCESS
      return roq::OrderStatus::UNDEFINED;
  }
  return {};
}

// static_assert(Helper{btse_futures::json::OrderStatus{btse_futures::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);

template <>
template <>
std::optional<roq::OrderStatus> Map<int32_t>::helper() const {
  return Helper{args_};
}

// int32_t => roq::OrderType

template <>
template <>
constexpr Helper<int32_t>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    case 0:  // UNDOC
      return roq::OrderType::UNDEFINED;
    case 76:  //  Limit Order
      return roq::OrderType::LIMIT;
    case 77:  //  Market Order
      return roq::OrderType::MARKET;
    case 80:  //  Algo orders
      return roq::OrderType::UNDEFINED;
  }
  return {};
}

static_assert(Helper{int32_t{76}} == roq::OrderType::LIMIT);
static_assert(Helper{int32_t{77}} == roq::OrderType::MARKET);
static_assert(Helper{int32_t{80}} == roq::OrderType::UNDEFINED);

template <>
template <>
std::optional<roq::OrderType> Map<int32_t>::helper() const {
  return Helper{args_};
}

// btse_futures::json::OrderState => roq::OrderStatus

template <>
template <>
constexpr Helper<btse_futures::json::OrderState>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum btse_futures::json::OrderState::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case STATUS_ACTIVE:
      return roq::OrderStatus::WORKING;
    case STATUS_INACTIVE:
      return roq::OrderStatus::SUSPENDED;
  }
  return {};
}

static_assert(Helper{btse_futures::json::OrderState{btse_futures::json::OrderState::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{btse_futures::json::OrderState{btse_futures::json::OrderState::STATUS_ACTIVE}} == roq::OrderStatus::WORKING);
static_assert(Helper{btse_futures::json::OrderState{btse_futures::json::OrderState::STATUS_INACTIVE}} == roq::OrderStatus::SUSPENDED);

template <>
template <>
std::optional<roq::OrderStatus> Map<btse_futures::json::OrderState>::helper() const {
  return Helper{args_};
}

// btse_futures::json::OrderType => roq::OrderType

template <>
template <>
constexpr Helper<btse_futures::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum btse_futures::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
    case OCO:
      return roq::OrderType::UNDEFINED;
  }
  return {};
}

static_assert(Helper{btse_futures::json::OrderType{btse_futures::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{btse_futures::json::OrderType{btse_futures::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{btse_futures::json::OrderType{btse_futures::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);
static_assert(Helper{btse_futures::json::OrderType{btse_futures::json::OrderType::OCO}} == roq::OrderType::UNDEFINED);

template <>
template <>
std::optional<roq::OrderType> Map<btse_futures::json::OrderType>::helper() const {
  return Helper{args_};
}

// btse_futures::json::Side => roq::Side

template <>
template <>
constexpr Helper<btse_futures::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum btse_futures::json::Side::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Side::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Side::UNDEFINED;
    case BUY:
      return roq::Side::BUY;
    case SELL:
      return roq::Side::SELL;
  }
  return {};
}

static_assert(Helper{btse_futures::json::Side{btse_futures::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{btse_futures::json::Side{btse_futures::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{btse_futures::json::Side{btse_futures::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<btse_futures::json::Side>::helper() const {
  return Helper{args_};
}

// btse_futures::json::TimeInForce => roq::TimeInForce

template <>
template <>
constexpr Helper<btse_futures::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum btse_futures::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case GTC:
      return roq::TimeInForce::GTC;
    case IOC:
      return roq::TimeInForce::IOC;
    case FOK:
      return roq::TimeInForce::FOK;
    case HALFMIN:
      return roq::TimeInForce::UNDEFINED;
    case FIVEMIN:
      return roq::TimeInForce::UNDEFINED;
    case HOUR:
      return roq::TimeInForce::UNDEFINED;
    case TWELVEHOUR:
      return roq::TimeInForce::UNDEFINED;
    case DAY:
      return roq::TimeInForce::UNDEFINED;
    case WEEK:
      return roq::TimeInForce::UNDEFINED;
    case MONTH:
      return roq::TimeInForce::UNDEFINED;
  }
  return {};
}

static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::FOK}} == roq::TimeInForce::FOK);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::HALFMIN}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::FIVEMIN}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::HOUR}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::TWELVEHOUR}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::DAY}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::WEEK}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::MONTH}} == roq::TimeInForce::UNDEFINED);

template <>
template <>
std::optional<roq::TimeInForce> Map<btse_futures::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// btse_futures::json::Type => roq::UpdateType

template <>
template <>
constexpr Helper<btse_futures::json::Type>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum btse_futures::json::Type::type_t;
    case UNDEFINED_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case SNAPSHOT:
      return roq::UpdateType::SNAPSHOT;
    case DELTA:
      return roq::UpdateType::INCREMENTAL;
  }
  return {};
}

static_assert(Helper{btse_futures::json::Type{btse_futures::json::Type::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{btse_futures::json::Type{btse_futures::json::Type::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{btse_futures::json::Type{btse_futures::json::Type::DELTA}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<btse_futures::json::Type>::helper() const {
  return Helper{args_};
}

// roq => btse_futures::json

// roq::OrderType => btse_futures::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<btse_futures::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return btse_futures::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return btse_futures::json::OrderType::MARKET;
    case LIMIT:
      return btse_futures::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == btse_futures::json::OrderType{btse_futures::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == btse_futures::json::OrderType{btse_futures::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == btse_futures::json::OrderType{btse_futures::json::OrderType::LIMIT});

template <>
template <>
std::optional<btse_futures::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// roq::Side => btse_futures::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<btse_futures::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return btse_futures::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return btse_futures::json::Side::BUY;
    case SELL:
      return btse_futures::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == btse_futures::json::Side{btse_futures::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == btse_futures::json::Side{btse_futures::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == btse_futures::json::Side{btse_futures::json::Side::SELL});

template <>
template <>
std::optional<btse_futures::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce => btse_futures::json::Force

template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<btse_futures::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return btse_futures::json::TimeInForce::GTC;
    case OPG:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return btse_futures::json::TimeInForce::IOC;
    case FOK:
      return btse_futures::json::TimeInForce::FOK;
    case GTX:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return btse_futures::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::TimeInForce::UNDEFINED} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFD} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::OPG} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::IOC} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::FOK});
static_assert(Helper{roq::TimeInForce::GTX} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTD} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_THE_CLOSE} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_THROUGH_CROSSING} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_CROSSING} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_FOR_TIME} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFA} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFM} == btse_futures::json::TimeInForce{btse_futures::json::TimeInForce::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<btse_futures::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
