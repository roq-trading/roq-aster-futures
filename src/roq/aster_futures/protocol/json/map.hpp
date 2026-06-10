/* Copyright (c) 2017-2026, Hans Erik Thrane */

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

#include "roq/aster_futures/protocol/json/action.hpp"
#include "roq/aster_futures/protocol/json/asset_mode.hpp"
#include "roq/aster_futures/protocol/json/category.hpp"
#include "roq/aster_futures/protocol/json/contract_type.hpp"
#include "roq/aster_futures/protocol/json/futures_type.hpp"
#include "roq/aster_futures/protocol/json/margin_mode.hpp"
#include "roq/aster_futures/protocol/json/message_type.hpp"
#include "roq/aster_futures/protocol/json/order_status.hpp"
#include "roq/aster_futures/protocol/json/order_type.hpp"
#include "roq/aster_futures/protocol/json/pos_side.hpp"
#include "roq/aster_futures/protocol/json/side.hpp"
#include "roq/aster_futures/protocol/json/time_in_force.hpp"
#include "roq/aster_futures/protocol/json/trade_scope.hpp"
#include "roq/aster_futures/protocol/json/trade_side.hpp"
#include "roq/aster_futures/protocol/json/trading_status.hpp"

namespace roq {

// aster_futures::protocol::json => roq

template <>
template <>
std::optional<UpdateType> Map<aster_futures::protocol::json::Action>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<aster_futures::protocol::json::AssetMode>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<aster_futures::protocol::json::Category, aster_futures::protocol::json::FuturesType>::helper() const;

template <>
template <>
std::optional<SecurityType> Map<aster_futures::protocol::json::ContractType>::helper() const;

template <>
template <>
std::optional<MarginMode> Map<aster_futures::protocol::json::MarginMode>::helper() const;

template <>
template <>
std::optional<UpdateType> Map<aster_futures::protocol::json::MessageType>::helper() const;

template <>
template <>
std::optional<OrderStatus> Map<aster_futures::protocol::json::OrderStatus>::helper() const;

template <>
template <>
std::optional<OrderType> Map<aster_futures::protocol::json::OrderType>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<aster_futures::protocol::json::PosSide, aster_futures::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<Side> Map<aster_futures::protocol::json::Side>::helper() const;

template <>
template <>
std::optional<TimeInForce> Map<aster_futures::protocol::json::TimeInForce>::helper() const;

template <>
template <>
std::optional<Liquidity> Map<aster_futures::protocol::json::TradeScope>::helper() const;

template <>
template <>
std::optional<PositionEffect> Map<aster_futures::protocol::json::TradeSide>::helper() const;

template <>
template <>
std::optional<TradingStatus> Map<aster_futures::protocol::json::TradingStatus>::helper() const;

// roq => aster_futures::protocol::json

template <>
template <>
std::optional<aster_futures::protocol::json::MarginMode> Map<MarginMode>::helper() const;

template <>
template <>
std::optional<aster_futures::protocol::json::OrderType> Map<OrderType>::helper() const;

template <>
template <>
std::optional<aster_futures::protocol::json::PosSide> Map<PositionEffect, Side>::helper() const;

template <>
template <>
std::optional<aster_futures::protocol::json::TradeSide> Map<PositionEffect>::helper() const;

template <>
template <>
std::optional<aster_futures::protocol::json::Side> Map<Side>::helper() const;

template <>
template <>
std::optional<aster_futures::protocol::json::TimeInForce> Map<TimeInForce>::helper() const;

}  // namespace roq
