/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/modify_order_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::ModifyOrderAck;

// note! truncated
TEST_CASE("simple", "[json_modify_order_ack]") {
  auto message = R"({)"
                 R"("orderId":33761731051,)"
                 R"("symbol":"BTCUSDT",)"
                 R"("status":"NEW",)"
                 R"("clientOrderId":"pAACYXLgl4QAAQAAAAAA",)"
                 R"("price":"49494",)"
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
                 R"("updateTime":1792637992500,)"
                 R"("newChainData":{)"
                 R"("hash":"0xe53940c02c565896080c37675d18972e3a9ce1b18b02396b4c3a3a1b3302d759")"
                 R"(},)"
                 R"("stpMode":"EXPIRE_MAKER")"
                 R"(})"sv;
  auto helper = [&](value_type &obj) {
    CHECK(obj.order_id == 33761731051);
    CHECK(obj.symbol == "BTCUSDT"sv);
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
