/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/ticker.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

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
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::Ticker obj{message, buffer};
}
