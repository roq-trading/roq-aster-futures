/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/book_ticker.hpp"

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
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::BookTicker obj{message, buffer};
}
