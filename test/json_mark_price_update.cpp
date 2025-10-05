/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/mark_price_update.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

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
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::MarkPriceUpdate obj{message, buffer};
}
