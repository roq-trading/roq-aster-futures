/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/mini_ticker.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_mini_ticker]") {
  auto message = R"({)"
                 R"("e":"24hrMiniTicker",)"
                 R"("E":1759571294392,)"
                 R"("s":"ETHUSDT",)"
                 R"("c":"4495.70",)"
                 R"("o":"4477.00",)"
                 R"("h":"4591.90",)"
                 R"("l":"4436.60",)"
                 R"("v":"4351982.886",)"
                 R"("q":"19582497608.36")"
                 R"(})";
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::MiniTicker obj{message, buffer};
}
