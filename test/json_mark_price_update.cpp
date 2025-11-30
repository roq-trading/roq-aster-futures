/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_mark_price_update]") {
  auto message = R"({)"
                 R"("e":"markPriceUpdate",)"
                 R"("E":1759543094000,)"
                 R"("s":"BTCUSDT",)"
                 R"("p":"121740.47017397",)"
                 R"("P":"121959.60845084",)"
                 R"("i":"121754.39043484",)"
                 R"("r":"0.00010000",)"
                 R"("T":1759564800000)"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  json::MarkPriceUpdate obj{message, buffers};
  CHECK(obj.event_type == json::EventType::MARK_PRICE_UPDATE);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &) override { FAIL(); }
    void operator()(Trace<json::MarkPriceUpdate> const &event) override {
      found = true;
      auto &[trace_info, mark_price_update] = event;
      CHECK(mark_price_update.event_type == json::EventType::MARK_PRICE_UPDATE);
    };
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
