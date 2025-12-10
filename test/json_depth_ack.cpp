/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/json/depth_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = json::DepthAck;

// note! reduced
TEST_CASE("simple", "[json_depth]") {
  auto message = R"({)"
                 R"("lastUpdateId":289171725544,)"
                 R"("E":1765381868437,)"
                 R"("T":1765381868422,)"
                 R"("bids":[)"
                 R"(["891.000","20.87"],)"
                 R"(["890.900","80.44"],)"
                 R"(["822.800","0.11"])"
                 R"(],)"
                 R"("asks":[)"
                 R"(["891.100","119.16"],)"
                 R"(["891.200","101.09"],)"
                 R"(["956.000","0.07"])"
                 R"(])"
                 R"(})";
  auto helper = [&](value_type &obj) { CHECK(obj.last_update_id == 289171725544); };
  core::json::BufferStack buffers{8192, 2};
  value_type obj{message, buffers};
  helper(obj);
}
