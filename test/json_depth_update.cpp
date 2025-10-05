/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/depth_update.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

// note! truncated
TEST_CASE("simple", "[json_depth_update]") {
  auto message = R"({)"
                 R"("e":"depthUpdate",)"
                 R"("E":1759573242499,)"
                 R"("T":1759573242384,)"
                 R"("s":"ETHUSDT",)"
                 R"("U":183522157812,)"
                 R"("u":183522159625,)"
                 R"("pu":183522154799,)"
                 R"("b":[)"
                 R"(["4496.80","103.712"],)"
                 R"(["4496.90","935.318"],)"
                 R"(["4497.00","0.000"],)"
                 R"(["4497.10","0.000"],)"
                 R"(["4497.20","0.000"])"
                 R"(],)"
                 R"("a":[)"
                 R"(["4497.00","774.371"],)"
                 R"(["4497.90","41.360"],)"
                 R"(["4498.10","15.855"],)"
                 R"(["4519.50","3.803"])"
                 R"(])"
                 R"(})";
  core::json::BufferStack buffer{8192, 2};
  [[maybe_unused]] json::DepthUpdate obj{message, buffer};
}
