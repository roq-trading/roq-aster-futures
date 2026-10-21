/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/user_trades_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::UserTradesAck;

TEST_CASE("empty", "[json_user_trades_ack]") {
  auto message = R"([)"
                 R"(])"sv;
  auto helper = [&](value_type &obj) { CHECK(std::empty(obj.data)); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
