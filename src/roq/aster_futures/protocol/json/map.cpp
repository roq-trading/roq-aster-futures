/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/protocol/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// aster_futures::protocol::json => roq

// aster_futures::protocol::json::ContractStatus => roq::TradingStatus

template <>
template <>
constexpr Helper<aster_futures::protocol::json::ContractStatus>::operator std::optional<roq::TradingStatus>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::ContractStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case PENDING_TRADING:
      return roq::TradingStatus::PRE_OPEN;
    case TRADING:
      return roq::TradingStatus::OPEN;
    case PRE_SETTLE:
      return roq::TradingStatus::PRE_CLOSE;
    case SETTLING:
      return roq::TradingStatus::CLOSE;
    case CLOSE:
      return roq::TradingStatus::CLOSE;
  }
  return {};
}

static_assert(
    Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::UNDEFINED_INTERNAL}} == roq::TradingStatus::UNDEFINED);
static_assert(
    Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::PENDING_TRADING}} == roq::TradingStatus::PRE_OPEN);
static_assert(Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::TRADING}} == roq::TradingStatus::OPEN);
static_assert(
    Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::PRE_SETTLE}} == roq::TradingStatus::PRE_CLOSE);
static_assert(Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::SETTLING}} == roq::TradingStatus::CLOSE);
static_assert(Helper{aster_futures::protocol::json::ContractStatus{aster_futures::protocol::json::ContractStatus::CLOSE}} == roq::TradingStatus::CLOSE);

template <>
template <>
std::optional<roq::TradingStatus> Map<aster_futures::protocol::json::ContractStatus>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::ContractType => roq::SecurityType

template <>
template <>
constexpr Helper<aster_futures::protocol::json::ContractType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::ContractType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case PERPETUAL:
      return roq::SecurityType::FUTURES;
  }
  return {};
}

static_assert(
    Helper{aster_futures::protocol::json::ContractType{aster_futures::protocol::json::ContractType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::ContractType{aster_futures::protocol::json::ContractType::PERPETUAL}} == roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<aster_futures::protocol::json::ContractType>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::FuturesType => roq::SecurityType

template <>
template <>
constexpr Helper<aster_futures::protocol::json::FuturesType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::FuturesType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case U_MARGINED:
      return roq::SecurityType::SWAP;  // ???
  }
  return {};
}

static_assert(
    Helper{aster_futures::protocol::json::FuturesType{aster_futures::protocol::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::FuturesType{aster_futures::protocol::json::FuturesType::U_MARGINED}} == roq::SecurityType::SWAP);

template <>
template <>
std::optional<roq::SecurityType> Map<aster_futures::protocol::json::FuturesType>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::MarginType => roq::MarginMode

template <>
template <>
constexpr Helper<aster_futures::protocol::json::MarginType>::operator std::optional<roq::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::MarginType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case CROSSED:
      return roq::MarginMode::CROSS;
    case ISOLATED:
      return roq::MarginMode::ISOLATED;
    case CROSS:
      return roq::MarginMode::CROSS;
  }
  return {};
}

static_assert(Helper{aster_futures::protocol::json::MarginType{aster_futures::protocol::json::MarginType::UNDEFINED_INTERNAL}} == roq::MarginMode::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::MarginType{aster_futures::protocol::json::MarginType::CROSSED}} == roq::MarginMode::CROSS);
static_assert(Helper{aster_futures::protocol::json::MarginType{aster_futures::protocol::json::MarginType::ISOLATED}} == roq::MarginMode::ISOLATED);
static_assert(Helper{aster_futures::protocol::json::MarginType{aster_futures::protocol::json::MarginType::CROSS}} == roq::MarginMode::CROSS);

template <>
template <>
std::optional<roq::MarginMode> Map<aster_futures::protocol::json::MarginType>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::OrderStatus => roq::OrderStatus

template <>
template <>
constexpr Helper<aster_futures::protocol::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::OrderStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case NEW:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELED:
      return roq::OrderStatus::CANCELED;
    case REJECTED:
      return roq::OrderStatus::REJECTED;
    case EXPIRED:
      return roq::OrderStatus::EXPIRED;
  }
  return {};
}

static_assert(
    Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::CANCELED}} == roq::OrderStatus::CANCELED);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::REJECTED}} == roq::OrderStatus::REJECTED);
static_assert(Helper{aster_futures::protocol::json::OrderStatus{aster_futures::protocol::json::OrderStatus::EXPIRED}} == roq::OrderStatus::EXPIRED);

template <>
template <>
std::optional<roq::OrderStatus> Map<aster_futures::protocol::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::OrderType => roq::OrderType

template <>
template <>
constexpr Helper<aster_futures::protocol::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case LIMIT:
      return roq::OrderType::LIMIT;
    case MARKET:
      return roq::OrderType::MARKET;
    case STOP:
      return roq::OrderType::LIMIT;
    case STOP_MARKET:
      return roq::OrderType::MARKET;
    case TAKE_PROFIT:
      return roq::OrderType::UNDEFINED;
    case TAKE_PROFIT_MARKET:
      return roq::OrderType::UNDEFINED;
    case TRAILING_STOP_MARKET:
      return roq::OrderType::UNDEFINED;
  }
  return {};
}

static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::STOP}} == roq::OrderType::LIMIT);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::STOP_MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::TAKE_PROFIT}} == roq::OrderType::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::TAKE_PROFIT_MARKET}} == roq::OrderType::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::TRAILING_STOP_MARKET}} == roq::OrderType::UNDEFINED);

template <>
template <>
std::optional<roq::OrderType> Map<aster_futures::protocol::json::OrderType>::helper() const {
  return Helper{args_};
}

// {aster_futures::protocol::json::PositionSide, aster_futures::protocol::json::Side} => roq::PositionEffect

template <>
template <>
constexpr Helper<aster_futures::protocol::json::PositionSide, aster_futures::protocol::json::Side>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::PositionSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case BOTH:
      return roq::PositionEffect::UNDEFINED;
    case LONG:
      switch (std::get<1>(args_)) {
        using enum aster_futures::protocol::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::OPEN;
        case SELL:
          return roq::PositionEffect::CLOSE;
      }
      break;
    case SHORT:
      switch (std::get<1>(args_)) {
        using enum aster_futures::protocol::json::Side::type_t;
        case UNDEFINED_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::PositionEffect::UNDEFINED;
        case BUY:
          return roq::PositionEffect::CLOSE;
        case SELL:
          return roq::PositionEffect::OPEN;
      }
      break;
  }
  return {};
}

static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::UNDEFINED_INTERNAL},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::UNDEFINED_INTERNAL}} == roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::BOTH},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::BUY}} == roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::BOTH},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::SELL}} == roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::LONG},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::BUY}} == roq::PositionEffect::OPEN);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::LONG},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::SELL}} == roq::PositionEffect::CLOSE);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::SHORT},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::BUY}} == roq::PositionEffect::CLOSE);
static_assert(
    Helper{
        aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::SHORT},
        aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::SELL}} == roq::PositionEffect::OPEN);

template <>
template <>
std::optional<roq::PositionEffect> Map<aster_futures::protocol::json::PositionSide, aster_futures::protocol::json::Side>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::Side => roq::Side

template <>
template <>
constexpr Helper<aster_futures::protocol::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::Side::type_t;
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

static_assert(Helper{aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<aster_futures::protocol::json::Side>::helper() const {
  return Helper{args_};
}

// aster_futures::protocol::json::TimeInForce => roq::TimeInForce

template <>
template <>
constexpr Helper<aster_futures::protocol::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::protocol::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case GTC:
      return roq::TimeInForce::GTC;
    case IOC:
      return roq::TimeInForce::IOC;
    case GTX:
      return roq::TimeInForce::GTX;
    case HIDDEN:
      return roq::TimeInForce::GTC;
  }
  return {};
}

static_assert(
    Helper{aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);
static_assert(Helper{aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::GTX}} == roq::TimeInForce::GTX);
static_assert(Helper{aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::HIDDEN}} == roq::TimeInForce::GTC);

template <>
template <>
std::optional<roq::TimeInForce> Map<aster_futures::protocol::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// roq => aster_futures::protocol::json

// roq::OrderType => aster_futures::protocol::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<aster_futures::protocol::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return aster_futures::protocol::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return aster_futures::protocol::json::OrderType::MARKET;
    case LIMIT:
      return aster_futures::protocol::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == aster_futures::protocol::json::OrderType{aster_futures::protocol::json::OrderType::LIMIT});

template <>
template <>
std::optional<aster_futures::protocol::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// {roq::PositionEffect, roq::Side} => aster_futures::protocol::json::PositionSide

template <>
template <>
constexpr Helper<roq::PositionEffect, roq::Side>::operator std::optional<aster_futures::protocol::json::PositionSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return aster_futures::protocol::json::PositionSide::BOTH;
    case OPEN:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return aster_futures::protocol::json::PositionSide::BOTH;
        case BUY:
          return aster_futures::protocol::json::PositionSide::LONG;
        case SELL:
          return aster_futures::protocol::json::PositionSide::SHORT;
      }
      break;
    case CLOSE:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return aster_futures::protocol::json::PositionSide::BOTH;
        case BUY:
          return aster_futures::protocol::json::PositionSide::SHORT;
        case SELL:
          return aster_futures::protocol::json::PositionSide::LONG;
      }
      break;
  }
  return {};
}

static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::UNDEFINED}, roq::Side{roq::Side::UNDEFINED}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::BOTH});
static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::OPEN}, roq::Side{roq::Side::UNDEFINED}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::BOTH});
static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::OPEN}, roq::Side{roq::Side::BUY}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::LONG});
static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::OPEN}, roq::Side{roq::Side::SELL}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::SHORT});
static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::CLOSE}, roq::Side{roq::Side::BUY}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::SHORT});
static_assert(
    Helper{roq::PositionEffect{roq::PositionEffect::CLOSE}, roq::Side{roq::Side::SELL}} ==
    aster_futures::protocol::json::PositionSide{aster_futures::protocol::json::PositionSide::LONG});

template <>
template <>
std::optional<aster_futures::protocol::json::PositionSide> Map<roq::PositionEffect, roq::Side>::helper() const {
  return Helper{args_};
}

// roq::Side => aster_futures::protocol::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<aster_futures::protocol::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return aster_futures::protocol::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return aster_futures::protocol::json::Side::BUY;
    case SELL:
      return aster_futures::protocol::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == aster_futures::protocol::json::Side{aster_futures::protocol::json::Side::SELL});

template <>
template <>
std::optional<aster_futures::protocol::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce => aster_futures::protocol::json::TimeInForce

template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<aster_futures::protocol::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return aster_futures::protocol::json::TimeInForce::GTC;
    case OPG:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return aster_futures::protocol::json::TimeInForce::IOC;
    case FOK:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTX:
      return aster_futures::protocol::json::TimeInForce::GTX;
    case GTD:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(
    Helper{roq::TimeInForce::UNDEFINED} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFD} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::OPG} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::IOC} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTX} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::GTX});
static_assert(Helper{roq::TimeInForce::GTD} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(
    Helper{roq::TimeInForce::AT_THE_CLOSE} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(
    Helper{roq::TimeInForce::GOOD_THROUGH_CROSSING} ==
    aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(
    Helper{roq::TimeInForce::AT_CROSSING} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(
    Helper{roq::TimeInForce::GOOD_FOR_TIME} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFA} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFM} == aster_futures::protocol::json::TimeInForce{aster_futures::protocol::json::TimeInForce::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<aster_futures::protocol::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
