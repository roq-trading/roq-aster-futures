/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/cancel_order_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::CancelOrderAck;

// note! truncated
TEST_CASE("simple", "[json_cancel_order_ack]") {
  auto message = R"({)"
                 R"("orderId":33748610399,)"
                 R"("symbol":"BTCUSDT",)"
                 R"("status":"CANCELED",)"
                 R"("clientOrderId":"LQACOLsSfIQAAQAAAAAA",)"
                 R"("price":"50000",)"
                 R"("avgPrice":"0.0000",)"
                 R"("origQty":"0.001",)"
                 R"("executedQty":"0",)"
                 R"("cumQty":"0",)"
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
                 R"("updateTime":1792592453200,)"
                 R"("cancelChainData":{)"
                 R"("hash":"0xc581ebebbac33be8c72273c8a0ad47cc21fc47aa59b7dc47c1675d96cfc43c49")"
                 R"(},)"
                 R"("stpMode":"EXPIRE_MAKER")"
                 R"(})"sv;
  auto helper = [&](value_type &obj) {
    CHECK(obj.order_id == 33748610399);
    CHECK(obj.symbol == "BTCUSDT"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
