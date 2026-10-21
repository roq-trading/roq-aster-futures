/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/open_orders_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::OpenOrdersAck;

TEST_CASE("empty", "[json_open_orders_ack]") {
  auto message = R"([)"
                 R"(])"sv;
  auto helper = [&](value_type &obj) { CHECK(std::empty(obj.data)); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}

TEST_CASE("simple", "[json_open_orders_ack]") {
  auto message = R"([{)"
                 R"("orderId":33745693585,)"
                 R"("symbol":"BTCUSDT",)"
                 R"("status":"NEW",)"
                 R"("clientOrderId":"wgAC2gmkdYQAAQAAAAAA",)"
                 R"("price":"50000",)"
                 R"("avgPrice":"0",)"
                 R"("origQty":"0.001",)"
                 R"("executedQty":"0",)"
                 R"("cumQuote":"0",)"
                 R"("timeInForce":"GTC",)"
                 R"("type":"LIMIT",)"
                 R"("reduceOnly":false,)"
                 R"("closePosition":false,)"
                 R"("side":"BUY",)"
                 R"("positionSide":"BOTH",)"
                 R"("stopPrice":"0",)"
                 R"("workingType":"CONTRACT_PRICE",)"
                 R"("priceProtect":false,)"
                 R"("origType":"LIMIT",)"
                 R"("time":1792580554026,)"
                 R"("updateTime":1792580554000,)"
                 R"("newChainData":{)"
                 R"("hash":"0x86eb980d0643069b42d18e6dff0adde49ff53387448beda9359e748e05379983")"
                 R"(},)"
                 R"("stpMode":"EXPIRE_MAKER")"
                 R"(})"
                 R"(])"sv;
  auto helper = [&](value_type &obj) { REQUIRE(std::size(obj.data) == 1); };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
