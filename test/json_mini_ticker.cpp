/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_mini_ticker]") {
  auto message = R"({)"
                 R"("e":"24hrMiniTicker",)"
                 R"("E":1759571294392,)"
                 R"("s":"ETHUSDT",)"
                 R"("c":"4495.70",)"
                 R"("o":"4477.00",)"
                 R"("h":"4591.90",)"
                 R"("l":"4436.60",)"
                 R"("v":"4351982.886",)"
                 R"("q":"19582497608.36")"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  json::MiniTicker obj{message, buffers};
  CHECK(obj.event_type == json::EventType::_24HR_MINI_TICKER);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &) override { FAIL(); }
    void operator()(Trace<json::MarkPriceUpdate> const &) override { FAIL(); }
    void operator()(Trace<json::MiniTicker> const &event) override {
      found = true;
      auto &[trace_info, mini_ticker] = event;
      CHECK(mini_ticker.event_type == json::EventType::_24HR_MINI_TICKER);
    };
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
