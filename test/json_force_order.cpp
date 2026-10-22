/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::ForceOrder;

TEST_CASE("simple", "[json_force_order]") {
  auto message = R"({)"
                 R"("e":"forceOrder",)"
                 R"("E":1792640015254,)"
                 R"("o":{)"
                 R"("s":"BTCUSDT",)"
                 R"("S":"SELL",)"
                 R"("o":"LIMIT",)"
                 R"("f":"IOC",)"
                 R"("q":"1.993",)"
                 R"("p":"82384.9",)"
                 R"("ap":"82641.6",)"
                 R"("X":"FILLED",)"
                 R"("l":"0.006",)"
                 R"("z":"1.993",)"
                 R"("T":1792640015200)"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::FORCE_ORDER); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
