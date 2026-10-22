/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::MarginCall;

TEST_CASE("simple", "[json_margin_call]") {
  auto message = R"({)"
                 R"("e":"MARGIN_CALL",)"
                 R"("E":1587727187525,)"
                 R"("cw":"3.16812045",)"
                 R"("p":[{)"
                 R"("s":"ETHUSDT",)"
                 R"("ps":"LONG",)"
                 R"("pa":"1.327",)"
                 R"("mt":"CROSSED",)"
                 R"("iw":"0",)"
                 R"("mp":"187.17127",)"
                 R"("up":"-1.166074",)"
                 R"("mm":"1.614445")"
                 R"(})"
                 R"(])"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::MARGIN_CALL); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
