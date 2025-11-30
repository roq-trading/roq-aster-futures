/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_depth_update]") {
  auto message = R"({)"
                 R"("e":"depthUpdate",)"
                 R"("E":1759573242499,)"
                 R"("T":1759573242384,)"
                 R"("s":"ETHUSDT",)"
                 R"("U":183522157812,)"
                 R"("u":183522159625,)"
                 R"("pu":183522154799,)"
                 R"("b":[)"
                 R"(["4496.80","103.712"],)"
                 R"(["4496.90","935.318"],)"
                 R"(["4497.00","0.000"],)"
                 R"(["4497.10","0.000"],)"
                 R"(["4497.20","0.000"])"
                 R"(],)"
                 R"("a":[)"
                 R"(["4497.00","774.371"],)"
                 R"(["4497.90","41.360"],)"
                 R"(["4498.10","15.855"],)"
                 R"(["4519.50","3.803"])"
                 R"(])"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::DepthUpdate obj{message, buffers};
  CHECK(obj.event_type == json::EventType::DEPTH_UPDATE);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &) { FAIL(); }
    void operator()(Trace<json::MarkPriceUpdate> const &) override { FAIL(); }
    void operator()(Trace<json::MiniTicker> const &) override { FAIL(); }
    void operator()(Trace<json::Ticker> const &) override { FAIL(); }
    void operator()(Trace<json::BookTicker> const &) override { FAIL(); }
    void operator()(Trace<json::DepthUpdate> const &event) override {
      found = true;
      auto &[trace_info, depth_update] = event;
      CHECK(depth_update.event_type == json::EventType::DEPTH_UPDATE);
    };
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
