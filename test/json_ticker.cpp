/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = json::Ticker;

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
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == json::EventType::_24HR_TICKER); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
