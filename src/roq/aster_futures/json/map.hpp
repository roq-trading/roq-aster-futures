/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/map.hpp"

#include "roq/liquidity.hpp"
#include "roq/margin_mode.hpp"
#include "roq/order_status.hpp"
#include "roq/order_type.hpp"
#include "roq/position_effect.hpp"
#include "roq/security_type.hpp"
#include "roq/side.hpp"
#include "roq/time_in_force.hpp"
#include "roq/trading_status.hpp"
#include "roq/update_type.hpp"

#include "roq/aster_futures/json/action.hpp"
#include "roq/aster_futures/json/asset_mode.hpp"
#include "roq/aster_futures/json/category.hpp"
#include "roq/aster_futures/json/contract_type.hpp"
#include "roq/aster_futures/json/futures_type.hpp"
#include "roq/aster_futures/json/margin_mode.hpp"
#include "roq/aster_futures/json/message_type.hpp"
#include "roq/aster_futures/json/order_status.hpp"
#include "roq/aster_futures/json/order_type.hpp"
#include "roq/aster_futures/json/pos_side.hpp"
#include "roq/aster_futures/json/side.hpp"
#include "roq/aster_futures/json/time_in_force.hpp"
#include "roq/aster_futures/json/trade_scope.hpp"
#include "roq/aster_futures/json/trade_side.hpp"
#include "roq/aster_futures/json/trading_status.hpp"

namespace roq {

// aster_futures::json => roq

template <>
template <>
std::optional<UpdateType> Map<aster_futures::json::Action>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<aster_futures::json::AssetMode>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<aster_futures::json::Category, aster_futures::json::FuturesType>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<aster_futures::json::ContractType>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<aster_futures::json::MarginMode>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<aster_futures::json::MessageType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<aster_futures::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<aster_futures::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<aster_futures::json::PosSide, aster_futures::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<aster_futures::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<aster_futures::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<aster_futures::json::TradeScope>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<aster_futures::json::TradeSide>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<aster_futures::json::TradingStatus>::helper() const;

// roq => aster_futures::json

template <>
template <>
std::optional<aster_futures::json::MarginMode> Map<MarginMode>::helper() const;

template <>
template <>
std::optional<aster_futures::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<aster_futures::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<aster_futures::json::TradeSide> Map<PositionEffect>::helper() const;

template <>
template <>
std::optional<aster_futures::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<aster_futures::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
