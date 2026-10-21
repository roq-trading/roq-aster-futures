/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/core/json/buffer_stack.hpp"

#include "roq/aster_futures/protocol/json/account_ack.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;
using namespace std::chrono_literals;

using namespace Catch::literals;

using value_type = protocol::json::AccountAck;

// note! truncated
TEST_CASE("simple", "[json_account_ack]") {
  auto message = R"({)"
                 R"("feeTier":0,)"
                 R"("canTrade":true,)"
                 R"("canDeposit":true,)"
                 R"("canWithdraw":true,)"
                 R"("updateTime":0,)"
                 R"("totalInitialMargin":"0.00000000",)"
                 R"("totalMaintMargin":"0.00000000",)"
                 R"("totalWalletBalance":"104.96968755",)"
                 R"("totalUnrealizedProfit":"0.00000000",)"
                 R"("totalMarginBalance":"104.96968755",)"
                 R"("totalPositionInitialMargin":"0.00000000",)"
                 R"("totalOpenOrderInitialMargin":"0.00000000",)"
                 R"("totalCrossWalletBalance":"104.96968755",)"
                 R"("totalCrossUnPnl":"0.00000000",)"
                 R"("availableBalance":"105.07950719",)"
                 R"("maxWithdrawAmount":"104.96968755",)"
                 R"("assets":[{)"
                 R"("asset":"CDL",)"
                 R"("walletBalance":"0.00000000",)"
                 R"("unrealizedProfit":"0.00000000",)"
                 R"("marginBalance":"0.00000000",)"
                 R"("maintMargin":"0.00000000",)"
                 R"("initialMargin":"0.00000000",)"
                 R"("positionInitialMargin":"0.00000000",)"
                 R"("openOrderInitialMargin":"0.00000000",)"
                 R"("maxWithdrawAmount":"0.00000000",)"
                 R"("crossWalletBalance":"0.00000000",)"
                 R"("crossUnPnl":"0.00000000",)"
                 R"("availableBalance":"26609.36503128",)"
                 R"("marginAvailable":true,)"
                 R"("updateTime":0)"
                 R"(},{)"
                 R"("asset":"ASBNB",)"
                 R"("walletBalance":"0.00000000",)"
                 R"("unrealizedProfit":"0.00000000",)"
                 R"("marginBalance":"0.00000000",)"
                 R"("maintMargin":"0.00000000",)"
                 R"("initialMargin":"0.00000000",)"
                 R"("positionInitialMargin":"0.00000000",)"
                 R"("openOrderInitialMargin":"0.00000000",)"
                 R"("maxWithdrawAmount":"0.00000000",)"
                 R"("crossWalletBalance":"0.00000000",)"
                 R"("crossUnPnl":"0.00000000",)"
                 R"("availableBalance":"0.12613198",)"
                 R"("marginAvailable":true,)"
                 R"("updateTime":0)"
                 R"(})"
                 R"(],)"
                 R"("positions":[{)"
                 R"("symbol":"SUSHIUSDT",)"
                 R"("initialMargin":"0",)"
                 R"("maintMargin":"0",)"
                 R"("unrealizedProfit":"0.00000000",)"
                 R"("positionInitialMargin":"0",)"
                 R"("openOrderInitialMargin":"0",)"
                 R"("leverage":"3",)"
                 R"("isolated":false,)"
                 R"("entryPrice":"0.0",)"
                 R"("maxNotional":"2500000",)"
                 R"("positionSide":"BOTH",)"
                 R"("positionAmt":"0.0",)"
                 R"("notional":"0",)"
                 R"("isolatedWallet":"0",)"
                 R"("updateTime":0)"
                 R"(},{)"
                 R"("symbol":"DUSKUSDT",)"
                 R"("initialMargin":"0",)"
                 R"("maintMargin":"0",)"
                 R"("unrealizedProfit":"0.00000000",)"
                 R"("positionInitialMargin":"0",)"
                 R"("openOrderInitialMargin":"0",)"
                 R"("leverage":"3",)"
                 R"("isolated":false,)"
                 R"("entryPrice":"0.0",)"
                 R"("maxNotional":"2500000",)"
                 R"("positionSide":"BOTH",)"
                 R"("positionAmt":"0",)"
                 R"("notional":"0",)"
                 R"("isolatedWallet":"0",)"
                 R"("updateTime":0)"
                 R"(})"
                 R"(])"
                 R"(})"sv;
  auto helper = [&](value_type &obj) {
    /*
    CHECK(obj.maker_commission == 10.0_a);
    CHECK(obj.taker_commission == 10.0_a);
    CHECK(obj.buyer_commission == 0.0_a);
    CHECK(obj.seller_commission == 0.0_a);
    CHECK(obj.can_trade == true);
    CHECK(obj.can_withdraw == true);
    CHECK(obj.can_deposit == true);
    CHECK(obj.update_time == 1620467758218ms);
    CHECK(obj.account_type == "MARGIN"sv);
    auto &balances = obj.balances;
    REQUIRE(std::size(balances) == 2);
    auto &balance_0 = balances[0];
    CHECK(balance_0.asset == "BTC"sv);
    CHECK(balance_0.free == 0.0_a);
    CHECK(balance_0.locked == 0.0_a);
    auto &balance_1 = balances[1];
    CHECK(balance_1.asset == "LTC"sv);
    CHECK(balance_1.free == 0.0_a);
    CHECK(balance_1.locked == 0.0_a);
    */
  };
  core::json::BufferStack buffers{8192, 1};
  value_type obj{message, buffers};
  helper(obj);
}
