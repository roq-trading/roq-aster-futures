/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/parser.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_book_ticker]") {
  auto message = R"({)"
                 R"("e":"bookTicker",)"
                 R"("u":183508045590,)"
                 R"("s":"BTCUSDT",)"
                 R"("b":"122231.0",)"
                 R"("B":"216.922",)"
                 R"("a":"122231.1",)"
                 R"("A":"64.312",)"
                 R"("T":1759572306715,)"
                 R"("E":1759572306716)"
                 R"(})";
  core::json::BufferStack buffers{8192, 2};
  // simple
  json::BookTicker obj{message, buffers};
  CHECK(obj.event_type == json::EventType::BOOK_TICKER);
  // parser
  struct MyHandler final : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) override { FAIL(); }
    void operator()(Trace<json::Ack> const &) override { FAIL(); }
    //
    void operator()(Trace<json::AggTrade> const &) override { FAIL(); }
    void operator()(Trace<json::MarkPriceUpdate> const &) override { FAIL(); }
    void operator()(Trace<json::MiniTicker> const &) override { FAIL(); }
    void operator()(Trace<json::Ticker> const &) override { FAIL(); }
    void operator()(Trace<json::BookTicker> const &event) override {
      found = true;
      auto &[trace_info, book_ticker] = event;
      CHECK(book_ticker.event_type == json::EventType::BOOK_TICKER);
    };
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
