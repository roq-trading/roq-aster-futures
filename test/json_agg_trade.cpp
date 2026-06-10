/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::AggTrade;

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
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::AGG_TRADE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
