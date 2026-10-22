/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/cancel_all_orders_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::CancelAllOrdersAck;

// note! truncated
TEST_CASE("simple", "[json_cancel_all_orders_ack]") {
  auto message = R"({)"
                 R"("code": 200,)"
                 R"("msg": "The operation of cancel all open order is done.")"
                 R"(})"sv;
  auto helper = [&](value_type &obj) {
    CHECK(obj.code == 200);
    CHECK(obj.msg == "The operation of cancel all open order is done."sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
