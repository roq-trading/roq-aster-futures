/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/order_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::OrderAck;

TEST_CASE("simple", "[json_order_ack]") {
  auto message = R"({)"
                 R"("orderId":33996044125,)"
                 R"("symbol":"BTCUSDT",)"
                 R"("status":"CANCELED",)"
                 R"("clientOrderId":"wQACZ4pWuIYAAQAAAAAA",)"
                 R"("price":"50",)"
                 R"("avgPrice":"0.0000",)"
                 R"("origQty":"1",)"
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
                 R"("time":1793551437219,)"
                 R"("updateTime":1793551441700,)"
                 R"("cancelChainData":{)"
                 R"("hash":"0xd8f5e506acd0ccd0e68cfe8397cb66ad96ac7f193e2ddd55eb1ef7a806bdedd9")"
                 R"(},)"
                 R"("stpMode":"EXPIRE_MAKER")"
                 R"(})"sv;
  auto helper = [&](value_type &obj) {
    CHECK(obj.order_id == 33996044125);
    CHECK(obj.symbol == "BTCUSDT"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
