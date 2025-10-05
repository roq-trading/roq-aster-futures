/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/agg_trade.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

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
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::AggTrade obj{message, buffer};
}
