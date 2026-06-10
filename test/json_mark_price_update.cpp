/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::MarkPriceUpdate;

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
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::MARK_PRICE_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
