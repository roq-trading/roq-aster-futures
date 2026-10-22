/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::ListenKeyExpired;

TEST_CASE("simple", "[json_listen_key_expired]") {
  auto message = R"({)"
                 R"("e":"listenKeyExpired",)"
                 R"("E":1792663498604,)"
                 R"("listenKey":"HpllR9GyUo4fDCvbGxaPQ9cofBmIn5L7e9Mdqm1LABvltUfEK4LUzbmCDscOpiFF")"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::LISTEN_KEY_EXPIRED); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
