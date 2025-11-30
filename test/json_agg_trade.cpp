/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_agg_trade]") {
  auto message = R"({)"
                 R"("e":"aggTrade",)"
                 R"("E":1759537956855,)"
                 R"("a":37632973,)"
                 R"("s":"BTCUSDT",)"
                 R"("p":"122056.3",)"
                 R"("q":"0.050",)"
                 R"("f":59922796,)"
                 R"("l":59922796,)"
                 R"("T":1759537956702,)"
                 R"("m":true)"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::AggTrade obj{message, buffers};
  CHECK(obj.event_type == json::EventType::AGG_TRADE);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &event) override {
      found = true;
      auto &[trace_info, agg_trade] = event;
      CHECK(agg_trade.event_type == json::EventType::AGG_TRADE);
    };
    void operator()(Trace<json::MarkPriceUpdate> const &) override { FAIL(); }
    void operator()(Trace<json::MiniTicker> const &) override { FAIL(); }
    void operator()(Trace<json::Ticker> const &) override { FAIL(); }
    void operator()(Trace<json::BookTicker> const &) override { FAIL(); }
    void operator()(Trace<json::DepthUpdate> const &) override { FAIL(); }
    //
    void operator()(Trace<json::Login> const &) override { FAIL(); }
    void operator()(Trace<json::Account> const &) override { FAIL(); }
    void operator()(Trace<json::Position> const &) override { FAIL(); }
    void operator()(Trace<json::Order> const &) override { FAIL(); }
    void operator()(Trace<json::Fill> const &) override { FAIL(); }

    bool found = {};
  } handler;
  auto res = json::Parser::dispatch(handler, message, buffers, {}, false);
  CHECK(res == true);
  CHECK(handler.found == true);
}
