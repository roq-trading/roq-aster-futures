/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <span>
#include <string_view>

#include "roq/trace_info.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/ack.hpp"   // ???
#include "roq/aster_futures/json/pong.hpp"  // ???

#include "roq/aster_futures/json/agg_trade.hpp"
#include "roq/aster_futures/json/book_ticker.hpp"
#include "roq/aster_futures/json/depth_update.hpp"
#include "roq/aster_futures/json/mark_price_update.hpp"
#include "roq/aster_futures/json/mini_ticker.hpp"
#include "roq/aster_futures/json/ticker.hpp"

#include "roq/aster_futures/json/login.hpp"

#include "roq/aster_futures/json/account.hpp"
#include "roq/aster_futures/json/fill.hpp"
#include "roq/aster_futures/json/order.hpp"
#include "roq/aster_futures/json/position.hpp"

namespace roq {
namespace aster_futures {
namespace json {

struct Parser final {
  struct Handler {
    virtual void operator()(Trace<json::Pong> const &) = 0;
    virtual void operator()(Trace<json::Ack> const &) = 0;
    //
    virtual void operator()(Trace<json::AggTrade> const &) = 0;
    virtual void operator()(Trace<json::MarkPriceUpdate> const &) = 0;
    virtual void operator()(Trace<json::MiniTicker> const &) = 0;
    virtual void operator()(Trace<json::Ticker> const &) = 0;
    virtual void operator()(Trace<json::BookTicker> const &) = 0;
    virtual void operator()(Trace<json::DepthUpdate> const &) = 0;
    //
    virtual void operator()(Trace<json::Login> const &) = 0;
    virtual void operator()(Trace<json::Account> const &) = 0;
    virtual void operator()(Trace<json::Position> const &) = 0;
    virtual void operator()(Trace<json::Order> const &) = 0;
    virtual void operator()(Trace<json::Fill> const &) = 0;
  };

  static bool dispatch(Handler &, std::string_view const &message, core::json::BufferStack &, TraceInfo const &, bool allow_unknown_event_types);
};

}  // namespace json
}  // namespace aster_futures
}  // namespace roq
