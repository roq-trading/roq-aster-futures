/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::BookTicker;

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
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::BOOK_TICKER); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
