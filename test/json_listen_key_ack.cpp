/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/listen_key_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::ListenKeyAck;

// note! truncated
TEST_CASE("simple", "[json_listen_key_ack]") {
  auto message = R"({)"
                 R"("listenKey":"DTucUc2Um6ftIZzy0bPG8edA3ywW88m9iA9tDAwXZsgILE7et5x6K0zkOdc5lB0Q")"
                 R"(})"sv;
  auto helper = [&](value_type &obj) { CHECK(obj.listen_key == "DTucUc2Um6ftIZzy0bPG8edA3ywW88m9iA9tDAwXZsgILE7et5x6K0zkOdc5lB0Q"sv); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
