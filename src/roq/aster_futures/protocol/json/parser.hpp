/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/pong.hpp"  // ???

// market data

#include "roq/aster_futures/protocol/json/ack.hpp"
#include "roq/aster_futures/protocol/json/agg_trade.hpp"
#include "roq/aster_futures/protocol/json/book_ticker.hpp"
#include "roq/aster_futures/protocol/json/depth_update.hpp"
#include "roq/aster_futures/protocol/json/force_order.hpp"
#include "roq/aster_futures/protocol/json/mark_price_update.hpp"
#include "roq/aster_futures/protocol/json/mini_ticker.hpp"
#include "roq/aster_futures/protocol/json/ticker.hpp"

// order management

#include "roq/aster_futures/protocol/json/account_config_update.hpp"
#include "roq/aster_futures/protocol/json/account_update.hpp"
#include "roq/aster_futures/protocol/json/listen_key_expired.hpp"
#include "roq/aster_futures/protocol/json/margin_call.hpp"
#include "roq/aster_futures/protocol/json/order_trade_update.hpp"

namespace roq {
namespace aster_futures {
namespace protocol {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Pong> const &) = 0;
    // - market data
    virtual void operator()(Trace<json::Ack> const &) = 0;
    virtual void operator()(Trace<json::AggTrade> const &) = 0;
    virtual void operator()(Trace<json::MarkPriceUpdate> const &) = 0;
    virtual void operator()(Trace<json::MiniTicker> const &) = 0;
    virtual void operator()(Trace<json::Ticker> const &) = 0;
    virtual void operator()(Trace<json::BookTicker> const &) = 0;
    virtual void operator()(Trace<json::DepthUpdate> const &) = 0;
    virtual void operator()(Trace<json::ForceOrder> const &) = 0;
    // - order management
    virtual void operator()(Trace<json::ListenKeyExpired> const &) = 0;
    virtual void operator()(Trace<json::MarginCall> const &) = 0;
    virtual void operator()(Trace<json::AccountUpdate> const &) = 0;
    virtual void operator()(Trace<json::OrderTradeUpdate> const &) = 0;
    virtual void operator()(Trace<json::AccountConfigUpdate> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace protocol
}  // namespace aster_futures
}  // namespace roq
