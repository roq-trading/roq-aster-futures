/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_ticker]") {
  auto message = R"({)"
                 R"("e":"24hrTicker",)"
                 R"("E":1759571795222,)"
                 R"("s":"BTCUSDT",)"
                 R"("p":"1897.5",)"
                 R"("P":"1.578",)"
                 R"("w":"121847.6",)"
                 R"("c":"122163.0",)"
                 R"("Q":"0.008",)"
                 R"("o":"120265.5",)"
                 R"("h":"123866.0",)"
                 R"("l":"119862.9",)"
                 R"("v":"604858.082",)"
                 R"("q":"73700494792.81",)"
                 R"("O":1759485360000,)"
                 R"("C":1759571795221,)"
                 R"("F":57124796,)"
                 R"("L":62090040,)"
                 R"("n":4965243)"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  json::Ticker obj{message, buffers};
  CHECK(obj.event_type == json::EventType::_24HR_TICKER);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &) override { FAIL(); }
    void operator()(Trace<json::MarkPriceUpdate> const &) override { FAIL(); }
    void operator()(Trace<json::MiniTicker> const &) override { FAIL(); }
    void operator()(Trace<json::Ticker> const &event) override {
      found = true;
      auto &[trace_info, ticker] = event;
      CHECK(ticker.event_type == json::EventType::_24HR_TICKER);
    };
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
