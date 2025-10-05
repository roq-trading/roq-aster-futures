/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/aster_futures/json/map.hpp"

using namespace std::literals;

namespace roq {

namespace {
template <typename... Args>
using Helper = detail::MapHelper<Args...>;
}

// aster_futures::json => roq

// aster_futures::json::Action => roq::MarginMode

template <>
template <>
constexpr Helper<aster_futures::json::Action>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::Action::type_t;
    case UNDEFINED_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case SNAPSHOT:
      return roq::UpdateType::SNAPSHOT;
    case UPDATE:
      return roq::UpdateType::INCREMENTAL;
  }
  return {};
}

static_assert(Helper{aster_futures::json::Action{aster_futures::json::Action::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{aster_futures::json::Action{aster_futures::json::Action::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{aster_futures::json::Action{aster_futures::json::Action::UPDATE}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<aster_futures::json::Action>::helper() const {
  return Helper{args_};
}

// aster_futures::json::AssetMode => roq::MarginMode

template <>
template <>
constexpr Helper<aster_futures::json::AssetMode>::operator std::optional<roq::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::AssetMode::type_t;
    case UNDEFINED_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case SINGLE:
      return roq::MarginMode::ISOLATED;
    case UNION:
      return roq::MarginMode::CROSS;
    case MULTI_ASSETS:
      return roq::MarginMode::CROSS;  // ???
  }
  return {};
}

static_assert(Helper{aster_futures::json::AssetMode{aster_futures::json::AssetMode::UNDEFINED_INTERNAL}} == roq::MarginMode::UNDEFINED);
static_assert(Helper{aster_futures::json::AssetMode{aster_futures::json::AssetMode::SINGLE}} == roq::MarginMode::ISOLATED);
static_assert(Helper{aster_futures::json::AssetMode{aster_futures::json::AssetMode::UNION}} == roq::MarginMode::CROSS);
static_assert(Helper{aster_futures::json::AssetMode{aster_futures::json::AssetMode::MULTI_ASSETS}} == roq::MarginMode::CROSS);

template <>
template <>
std::optional<roq::MarginMode> Map<aster_futures::json::AssetMode>::helper() const {
  return Helper{args_};
}

// {aster_futures::json::Category, aster_futures::json::FuturesType} => roq::SecurityType

template <>
template <>
constexpr Helper<aster_futures::json::Category, aster_futures::json::FuturesType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::Category::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case SPOT:
      return roq::SecurityType::SPOT;
    case MARGIN:
      return roq::SecurityType::SPOT;
    case USDT_FUTURES:
    case USDC_FUTURES:
    case COIN_FUTURES:
      switch (std::get<1>(args_)) {
        using enum aster_futures::json::FuturesType::type_t;
        case UNDEFINED_INTERNAL:
          return roq::SecurityType::UNDEFINED;
        case UNKNOWN_INTERNAL:
          return roq::SecurityType::UNDEFINED;
        case PERPETUAL:
          return roq::SecurityType::SWAP;
        case DELIVERY:
          return roq::SecurityType::FUTURES;
      }
      break;
  }
  return {};
}

static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::UNDEFINED_INTERNAL},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::SPOT},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::SPOT);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::MARGIN},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::SPOT);

static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDT_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDT_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNKNOWN_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDT_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::PERPETUAL}} == roq::SecurityType::SWAP);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDT_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::DELIVERY}} == roq::SecurityType::FUTURES);

static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDC_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDC_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNKNOWN_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDC_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::PERPETUAL}} == roq::SecurityType::SWAP);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::USDC_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::DELIVERY}} == roq::SecurityType::FUTURES);

static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::COIN_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::COIN_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::UNKNOWN_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::COIN_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::PERPETUAL}} == roq::SecurityType::SWAP);
static_assert(
    Helper{
        aster_futures::json::Category{aster_futures::json::Category::COIN_FUTURES},
        aster_futures::json::FuturesType{aster_futures::json::FuturesType::DELIVERY}} == roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<aster_futures::json::Category, aster_futures::json::FuturesType>::helper() const {
  return Helper{args_};
}

// aster_futures::json::ContractType => roq::SecurityType

template <>
template <>
constexpr Helper<aster_futures::json::ContractType>::operator std::optional<roq::SecurityType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::ContractType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::SecurityType::UNDEFINED;
    case PERPETUAL:
      return roq::SecurityType::FUTURES;
  }
  return {};
}

static_assert(Helper{aster_futures::json::ContractType{aster_futures::json::ContractType::UNDEFINED_INTERNAL}} == roq::SecurityType::UNDEFINED);
static_assert(Helper{aster_futures::json::ContractType{aster_futures::json::ContractType::PERPETUAL}} == roq::SecurityType::FUTURES);

template <>
template <>
std::optional<roq::SecurityType> Map<aster_futures::json::ContractType>::helper() const {
  return Helper{args_};
}

// aster_futures::json::MarginMode => roq::MarginMode

template <>
template <>
constexpr Helper<aster_futures::json::MarginMode>::operator std::optional<roq::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::MarginMode::type_t;
    case UNDEFINED_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::MarginMode::UNDEFINED;
    case CROSSED:
      return roq::MarginMode::CROSS;
    case ISOLATED:
      return roq::MarginMode::ISOLATED;
  }
  return {};
}

static_assert(Helper{aster_futures::json::MarginMode{aster_futures::json::MarginMode::UNDEFINED_INTERNAL}} == roq::MarginMode::UNDEFINED);
static_assert(Helper{aster_futures::json::MarginMode{aster_futures::json::MarginMode::CROSSED}} == roq::MarginMode::CROSS);
static_assert(Helper{aster_futures::json::MarginMode{aster_futures::json::MarginMode::ISOLATED}} == roq::MarginMode::ISOLATED);

template <>
template <>
std::optional<roq::MarginMode> Map<aster_futures::json::MarginMode>::helper() const {
  return Helper{args_};
}

// aster_futures::json::MessageType => roq::UpdateType

template <>
template <>
constexpr Helper<aster_futures::json::MessageType>::operator std::optional<roq::UpdateType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::MessageType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::UpdateType::UNDEFINED;
    case SNAPSHOT:
      return roq::UpdateType::SNAPSHOT;
    case INCREMENTAL:
      return roq::UpdateType::INCREMENTAL;
  }
  return {};
}

static_assert(Helper{aster_futures::json::MessageType{aster_futures::json::MessageType::UNDEFINED_INTERNAL}} == roq::UpdateType::UNDEFINED);
static_assert(Helper{aster_futures::json::MessageType{aster_futures::json::MessageType::SNAPSHOT}} == roq::UpdateType::SNAPSHOT);
static_assert(Helper{aster_futures::json::MessageType{aster_futures::json::MessageType::INCREMENTAL}} == roq::UpdateType::INCREMENTAL);

template <>
template <>
std::optional<roq::UpdateType> Map<aster_futures::json::MessageType>::helper() const {
  return Helper{args_};
}

// aster_futures::json::OrderStatus => roq::OrderStatus

template <>
template <>
constexpr Helper<aster_futures::json::OrderStatus>::operator std::optional<roq::OrderStatus>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::OrderStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderStatus::UNDEFINED;
    case LIVE:
      return roq::OrderStatus::WORKING;
    case NEW:
      return roq::OrderStatus::WORKING;
    case PARTIALLY_FILLED:
      return roq::OrderStatus::WORKING;
    case FILLED:
      return roq::OrderStatus::COMPLETED;
    case CANCELLED:
      return roq::OrderStatus::CANCELED;
  }
  return {};
}

static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::UNDEFINED_INTERNAL}} == roq::OrderStatus::UNDEFINED);
static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::LIVE}} == roq::OrderStatus::WORKING);
static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::NEW}} == roq::OrderStatus::WORKING);
static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::PARTIALLY_FILLED}} == roq::OrderStatus::WORKING);
static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::FILLED}} == roq::OrderStatus::COMPLETED);
static_assert(Helper{aster_futures::json::OrderStatus{aster_futures::json::OrderStatus::CANCELLED}} == roq::OrderStatus::CANCELED);

template <>
template <>
std::optional<roq::OrderStatus> Map<aster_futures::json::OrderStatus>::helper() const {
  return Helper{args_};
}

// aster_futures::json::OrderType => roq::OrderType

template <>
template <>
constexpr Helper<aster_futures::json::OrderType>::operator std::optional<roq::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::OrderType::type_t;
    case UNDEFINED_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::OrderType::UNDEFINED;
    case MARKET:
      return roq::OrderType::MARKET;
    case LIMIT:
      return roq::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{aster_futures::json::OrderType{aster_futures::json::OrderType::UNDEFINED_INTERNAL}} == roq::OrderType::UNDEFINED);
static_assert(Helper{aster_futures::json::OrderType{aster_futures::json::OrderType::MARKET}} == roq::OrderType::MARKET);
static_assert(Helper{aster_futures::json::OrderType{aster_futures::json::OrderType::LIMIT}} == roq::OrderType::LIMIT);

template <>
template <>
std::optional<roq::OrderType> Map<aster_futures::json::OrderType>::helper() const {
  return Helper{args_};
}

// {aster_futures::json::PosSide, aster_futures::json::Side} => roq::PositionEffect

template <>
template <>
constexpr Helper<aster_futures::json::PosSide, aster_futures::json::Side>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::PosSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case LONG:
      switch (std::get<1>(args_)) {
        using enum aster_futures::json::Side::type_t;
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
        using enum aster_futures::json::Side::type_t;
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
    case NET:
      return roq::PositionEffect::UNDEFINED;
  }
  return {};
}

static_assert(
    Helper{
        aster_futures::json::PosSide{aster_futures::json::PosSide::UNDEFINED_INTERNAL},
        aster_futures::json::Side{aster_futures::json::Side::UNDEFINED_INTERNAL}} == roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::LONG}, aster_futures::json::Side{aster_futures::json::Side::BUY}} ==
    roq::PositionEffect::OPEN);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::LONG}, aster_futures::json::Side{aster_futures::json::Side::SELL}} ==
    roq::PositionEffect::CLOSE);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::SHORT}, aster_futures::json::Side{aster_futures::json::Side::BUY}} ==
    roq::PositionEffect::CLOSE);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::SHORT}, aster_futures::json::Side{aster_futures::json::Side::SELL}} ==
    roq::PositionEffect::OPEN);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::NET}, aster_futures::json::Side{aster_futures::json::Side::BUY}} ==
    roq::PositionEffect::UNDEFINED);
static_assert(
    Helper{aster_futures::json::PosSide{aster_futures::json::PosSide::NET}, aster_futures::json::Side{aster_futures::json::Side::SELL}} ==
    roq::PositionEffect::UNDEFINED);

template <>
template <>
std::optional<roq::PositionEffect> Map<aster_futures::json::PosSide, aster_futures::json::Side>::helper() const {
  return Helper{args_};
}

// aster_futures::json::Side => roq::Side

template <>
template <>
constexpr Helper<aster_futures::json::Side>::operator std::optional<roq::Side>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::Side::type_t;
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

static_assert(Helper{aster_futures::json::Side{aster_futures::json::Side::UNDEFINED_INTERNAL}} == roq::Side::UNDEFINED);
static_assert(Helper{aster_futures::json::Side{aster_futures::json::Side::BUY}} == roq::Side::BUY);
static_assert(Helper{aster_futures::json::Side{aster_futures::json::Side::SELL}} == roq::Side::SELL);

template <>
template <>
std::optional<roq::Side> Map<aster_futures::json::Side>::helper() const {
  return Helper{args_};
}

// aster_futures::json::TimeInForce => roq::TimeInForce

template <>
template <>
constexpr Helper<aster_futures::json::TimeInForce>::operator std::optional<roq::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::TimeInForce::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TimeInForce::UNDEFINED;
    case IOC:
      return roq::TimeInForce::IOC;
    case FOK:
      return roq::TimeInForce::FOK;
    case GTC:
      return roq::TimeInForce::GTC;
    case POST_ONLY:
      return roq::TimeInForce::GTC;
  }
  return {};
}

static_assert(Helper{aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL}} == roq::TimeInForce::UNDEFINED);
static_assert(Helper{aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::IOC}} == roq::TimeInForce::IOC);
static_assert(Helper{aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::FOK}} == roq::TimeInForce::FOK);
static_assert(Helper{aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::GTC}} == roq::TimeInForce::GTC);
static_assert(Helper{aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::POST_ONLY}} == roq::TimeInForce::GTC);

template <>
template <>
std::optional<roq::TimeInForce> Map<aster_futures::json::TimeInForce>::helper() const {
  return Helper{args_};
}

// aster_futures::json::TradeScope => roq::Liquidity

template <>
template <>
constexpr Helper<aster_futures::json::TradeScope>::operator std::optional<roq::Liquidity>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::TradeScope::type_t;
    case UNDEFINED_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::Liquidity::UNDEFINED;
    case TAKER:
      return roq::Liquidity::TAKER;
    case MAKER:
      return roq::Liquidity::MAKER;
  }
  return {};
}

static_assert(Helper{aster_futures::json::TradeScope{aster_futures::json::TradeScope::UNDEFINED_INTERNAL}} == roq::Liquidity::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeScope{aster_futures::json::TradeScope::TAKER}} == roq::Liquidity::TAKER);
static_assert(Helper{aster_futures::json::TradeScope{aster_futures::json::TradeScope::MAKER}} == roq::Liquidity::MAKER);

template <>
template <>
std::optional<roq::Liquidity> Map<aster_futures::json::TradeScope>::helper() const {
  return Helper{args_};
}

// aster_futures::json::TradeSide => roq::PositionEffect

template <>
template <>
constexpr Helper<aster_futures::json::TradeSide>::operator std::optional<roq::PositionEffect>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::TradeSide::type_t;
    case UNDEFINED_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::PositionEffect::UNDEFINED;
    case CLOSE:
      return roq::PositionEffect::CLOSE;
    case OPEN:
      return roq::PositionEffect::OPEN;
    case REDUCE_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case REDUCE_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case BURST_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case BURST_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case OFFSET_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case OFFSET_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case DELIVERY_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case DELIVERY_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case DTE_SYS_ADL_CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
    case DTE_SYS_ADL_CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case REDUCE_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case REDUCE_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case BURST_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case BURST_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DELIVERY_SELL_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DELIVERY_BUY_SINGLE:
      return roq::PositionEffect::UNDEFINED;
    case DTE_SYS_ADL_BUY_IN_SINGLE_SIDE_MODE:
      return roq::PositionEffect::UNDEFINED;
    case DTE_SYS_ADL_SELL_IN_SINGLE_SIDE_MODE:
      return roq::PositionEffect::UNDEFINED;
    case OPEN_SHORT:
      return roq::PositionEffect::OPEN;
    case OPEN_LONG:
      return roq::PositionEffect::OPEN;
    case CLOSE_SHORT:
      return roq::PositionEffect::CLOSE;
    case CLOSE_LONG:
      return roq::PositionEffect::CLOSE;
  }
  return {};
}

static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::UNDEFINED_INTERNAL}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::CLOSE}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::OPEN}} == roq::PositionEffect::OPEN);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::REDUCE_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::REDUCE_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::BURST_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::BURST_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::OFFSET_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::OFFSET_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DELIVERY_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DELIVERY_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DTE_SYS_ADL_CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DTE_SYS_ADL_CLOSE_SHORT}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::REDUCE_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::REDUCE_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::BURST_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::BURST_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DELIVERY_SELL_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DELIVERY_BUY_SINGLE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DTE_SYS_ADL_BUY_IN_SINGLE_SIDE_MODE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::DTE_SYS_ADL_SELL_IN_SINGLE_SIDE_MODE}} == roq::PositionEffect::UNDEFINED);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::OPEN_SHORT}} == roq::PositionEffect::OPEN);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::CLOSE_LONG}} == roq::PositionEffect::CLOSE);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::OPEN_SHORT}} == roq::PositionEffect::OPEN);
static_assert(Helper{aster_futures::json::TradeSide{aster_futures::json::TradeSide::CLOSE_LONG}} == roq::PositionEffect::CLOSE);

template <>
template <>
std::optional<roq::PositionEffect> Map<aster_futures::json::TradeSide>::helper() const {
  return Helper{args_};
}

// aster_futures::json::TradingStatus => roq::TradingStatus

template <>
template <>
constexpr Helper<aster_futures::json::TradingStatus>::operator std::optional<roq::TradingStatus>() const {
  switch (std::get<0>(args_)) {
    using enum aster_futures::json::TradingStatus::type_t;
    case UNDEFINED_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case UNKNOWN_INTERNAL:
      return roq::TradingStatus::UNDEFINED;
    case TRADING:
      return roq::TradingStatus::OPEN;
    case PENDING_TRADING:
      return roq::TradingStatus::PRE_OPEN;
    case SETTLING:
      return roq::TradingStatus::CLOSE;
  }
  return {};
}

static_assert(Helper{aster_futures::json::TradingStatus{aster_futures::json::TradingStatus::UNDEFINED_INTERNAL}} == roq::TradingStatus::UNDEFINED);
static_assert(Helper{aster_futures::json::TradingStatus{aster_futures::json::TradingStatus::TRADING}} == roq::TradingStatus::OPEN);
static_assert(Helper{aster_futures::json::TradingStatus{aster_futures::json::TradingStatus::PENDING_TRADING}} == roq::TradingStatus::PRE_OPEN);
static_assert(Helper{aster_futures::json::TradingStatus{aster_futures::json::TradingStatus::SETTLING}} == roq::TradingStatus::CLOSE);

template <>
template <>
std::optional<roq::TradingStatus> Map<aster_futures::json::TradingStatus>::helper() const {
  return Helper{args_};
}

// roq => aster_futures::json

// roq::MarginMode => aster_futures::json::MarginMode

template <>
template <>
constexpr Helper<roq::MarginMode>::operator std::optional<aster_futures::json::MarginMode>() const {
  switch (std::get<0>(args_)) {
    using enum roq::MarginMode;
    case UNDEFINED:
      return aster_futures::json::MarginMode::UNDEFINED_INTERNAL;
    case CROSS:
      return aster_futures::json::MarginMode::CROSSED;
    case ISOLATED:
      return aster_futures::json::MarginMode::ISOLATED;
    case PORTFOLIO:
      return aster_futures::json::MarginMode::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::MarginMode::UNDEFINED} == aster_futures::json::MarginMode{aster_futures::json::MarginMode::UNDEFINED_INTERNAL});
static_assert(Helper{roq::MarginMode::CROSS} == aster_futures::json::MarginMode{aster_futures::json::MarginMode::CROSSED});
static_assert(Helper{roq::MarginMode::ISOLATED} == aster_futures::json::MarginMode{aster_futures::json::MarginMode::ISOLATED});
static_assert(Helper{roq::MarginMode::PORTFOLIO} == aster_futures::json::MarginMode{aster_futures::json::MarginMode::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<aster_futures::json::MarginMode> Map<roq::MarginMode>::helper() const {
  return Helper{args_};
}

// roq::OrderType => aster_futures::json::OrderType

template <>
template <>
constexpr Helper<roq::OrderType>::operator std::optional<aster_futures::json::OrderType>() const {
  switch (std::get<0>(args_)) {
    using enum roq::OrderType;
    case UNDEFINED:
      return aster_futures::json::OrderType::UNDEFINED_INTERNAL;
    case MARKET:
      return aster_futures::json::OrderType::MARKET;
    case LIMIT:
      return aster_futures::json::OrderType::LIMIT;
  }
  return {};
}

static_assert(Helper{roq::OrderType::UNDEFINED} == aster_futures::json::OrderType{aster_futures::json::OrderType::UNDEFINED_INTERNAL});
static_assert(Helper{roq::OrderType::MARKET} == aster_futures::json::OrderType{aster_futures::json::OrderType::MARKET});
static_assert(Helper{roq::OrderType::LIMIT} == aster_futures::json::OrderType{aster_futures::json::OrderType::LIMIT});

template <>
template <>
std::optional<aster_futures::json::OrderType> Map<roq::OrderType>::helper() const {
  return Helper{args_};
}

// {roq::PositionEffect, roq::Side} => aster_futures::json::PosSide

template <>
template <>
constexpr Helper<roq::PositionEffect, roq::Side>::operator std::optional<aster_futures::json::PosSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return aster_futures::json::PosSide::UNDEFINED_INTERNAL;
    case OPEN:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return aster_futures::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return aster_futures::json::PosSide::LONG;
        case SELL:
          return aster_futures::json::PosSide::SHORT;
      }
      break;
    case CLOSE:
      switch (std::get<1>(args_)) {
        using enum roq::Side;
        case UNDEFINED:
          return aster_futures::json::PosSide::UNDEFINED_INTERNAL;
        case BUY:
          return aster_futures::json::PosSide::SHORT;
        case SELL:
          return aster_futures::json::PosSide::LONG;
      }
      break;
  }
  return {};
}

static_assert(Helper{roq::PositionEffect::UNDEFINED, roq::Side::UNDEFINED} == aster_futures::json::PosSide{aster_futures::json::PosSide::UNDEFINED_INTERNAL});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::BUY} == aster_futures::json::PosSide{aster_futures::json::PosSide::LONG});
static_assert(Helper{roq::PositionEffect::OPEN, roq::Side::SELL} == aster_futures::json::PosSide{aster_futures::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::BUY} == aster_futures::json::PosSide{aster_futures::json::PosSide::SHORT});
static_assert(Helper{roq::PositionEffect::CLOSE, roq::Side::SELL} == aster_futures::json::PosSide{aster_futures::json::PosSide::LONG});

template <>
template <>
std::optional<aster_futures::json::PosSide> Map<roq::PositionEffect, roq::Side>::helper() const {
  return Helper{args_};
}

// roq::PositionEffect => aster_futures::json::TradeSide

template <>
template <>
constexpr Helper<roq::PositionEffect>::operator std::optional<aster_futures::json::TradeSide>() const {
  switch (std::get<0>(args_)) {
    using enum roq::PositionEffect;
    case UNDEFINED:
      return aster_futures::json::TradeSide::UNDEFINED_INTERNAL;
    case OPEN:
      return aster_futures::json::TradeSide::OPEN;
    case CLOSE:
      return aster_futures::json::TradeSide::CLOSE;
  }
  return {};
}

static_assert(Helper{roq::PositionEffect::UNDEFINED} == aster_futures::json::TradeSide{aster_futures::json::TradeSide::UNDEFINED_INTERNAL});
static_assert(Helper{roq::PositionEffect::OPEN} == aster_futures::json::TradeSide{aster_futures::json::TradeSide::OPEN});
static_assert(Helper{roq::PositionEffect::CLOSE} == aster_futures::json::TradeSide{aster_futures::json::TradeSide::CLOSE});

template <>
template <>
std::optional<aster_futures::json::TradeSide> Map<roq::PositionEffect>::helper() const {
  return Helper{args_};
}

// roq::Side => aster_futures::json::Side

template <>
template <>
constexpr Helper<roq::Side>::operator std::optional<aster_futures::json::Side>() const {
  switch (std::get<0>(args_)) {
    using enum roq::Side;
    case UNDEFINED:
      return aster_futures::json::Side::UNDEFINED_INTERNAL;
    case BUY:
      return aster_futures::json::Side::BUY;
    case SELL:
      return aster_futures::json::Side::SELL;
  }
  return {};
}

static_assert(Helper{roq::Side::UNDEFINED} == aster_futures::json::Side{aster_futures::json::Side::UNDEFINED_INTERNAL});
static_assert(Helper{roq::Side::BUY} == aster_futures::json::Side{aster_futures::json::Side::BUY});
static_assert(Helper{roq::Side::SELL} == aster_futures::json::Side{aster_futures::json::Side::SELL});

template <>
template <>
std::optional<aster_futures::json::Side> Map<roq::Side>::helper() const {
  return Helper{args_};
}

// roq::TimeInForce => aster_futures::json::Force

// POST_ONLY ???
template <>
template <>
constexpr Helper<roq::TimeInForce>::operator std::optional<aster_futures::json::TimeInForce>() const {
  switch (std::get<0>(args_)) {
    using enum roq::TimeInForce;
    case UNDEFINED:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFD:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTC:
      return aster_futures::json::TimeInForce::GTC;
    case OPG:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case IOC:
      return aster_futures::json::TimeInForce::IOC;
    case FOK:
      return aster_futures::json::TimeInForce::FOK;
    case GTX:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GTD:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_THE_CLOSE:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_THROUGH_CROSSING:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case AT_CROSSING:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GOOD_FOR_TIME:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFA:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
    case GFM:
      return aster_futures::json::TimeInForce::UNDEFINED_INTERNAL;
  }
  return {};
}

static_assert(Helper{roq::TimeInForce::UNDEFINED} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFD} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTC} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::GTC});
static_assert(Helper{roq::TimeInForce::OPG} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::IOC} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::IOC});
static_assert(Helper{roq::TimeInForce::FOK} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::FOK});
static_assert(Helper{roq::TimeInForce::GTX} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GTD} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_THE_CLOSE} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_THROUGH_CROSSING} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::AT_CROSSING} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GOOD_FOR_TIME} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFA} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});
static_assert(Helper{roq::TimeInForce::GFM} == aster_futures::json::TimeInForce{aster_futures::json::TimeInForce::UNDEFINED_INTERNAL});

template <>
template <>
std::optional<aster_futures::json::TimeInForce> Map<roq::TimeInForce>::helper() const {
  return Helper{args_};
}

}  // namespace roq
