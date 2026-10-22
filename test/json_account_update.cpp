/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::AccountUpdate;

TEST_CASE("docs", "[json_account_update]") {
  auto message = R"({)"
                 R"("e": "ACCOUNT_UPDATE",)"
                 R"("E": 1564745798939,)"
                 R"("T": 1564745798938,)"
                 R"("a":{)"
                 R"("m":"ORDER",)"
                 R"("B":[{)"
                 R"("a":"USDT",)"
                 R"("wb":"122624.12345678",)"
                 R"("cw":"100.12345678",)"
                 R"("bc":"50.12345678")"
                 R"(},{)"
                 R"("a":"BUSD",   )"
                 R"("wb":"1.00000000",)"
                 R"("cw":"0.00000000",   )"
                 R"("bc":"-49.12345678")"
                 R"(})"
                 R"(],)"
                 R"("P":[{)"
                 R"("s":"BTCUSDT",)"
                 R"("pa":"0",)"
                 R"("ep":"0.00000",)"
                 R"("cr":"200",)"
                 R"("up":"0",)"
                 R"("mt":"isolated",)"
                 R"("iw":"0.00000000",)"
                 R"("ps":"BOTH")"
                 R"(},{)"
                 R"("s":"BTCUSDT",)"
                 R"("pa":"20",)"
                 R"("ep":"6563.66500",)"
                 R"("cr":"0",)"
                 R"("up":"2850.21200",)"
                 R"("mt":"isolated",)"
                 R"("iw":"13200.70726908",)"
                 R"("ps":"LONG")"
                 R"(},{)"
                 R"("s":"BTCUSDT",)"
                 R"("pa":"-10",)"
                 R"("ep":"6563.86000",)"
                 R"("cr":"-45.04000000",)"
                 R"("up":"-1423.15600",)"
                 R"("mt":"isolated",)"
                 R"("iw":"6570.42511771",)"
                 R"("ps":"SHORT")"
                 R"(})"
                 R"(])"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ACCOUNT_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("fill", "[json_account_update]") {
  auto message = R"({)"
                 R"("e":"ACCOUNT_UPDATE",)"
                 R"("T":1792671901350,)"
                 R"("E":1792671901389,)"
                 R"("a":{)"
                 R"("B":[{)"
                 R"("a":"USDT",)"
                 R"("wb":"-0.03271312",)"
                 R"("cw":"-0.03271312",)"
                 R"("bc":"0")"
                 R"(})"
                 R"(],)"
                 R"("P":[{)"
                 R"("s":"BTCUSDT",)"
                 R"("pa":"0.001",)"
                 R"("ep":"81782.80000000",)"
                 R"("cr":"0",)"
                 R"("up":"0.02600000",)"
                 R"("mt":"cross",)"
                 R"("iw":"0",)"
                 R"("ps":"BOTH",)"
                 R"("ma":"USDT")"
                 R"(})"
                 R"(],)"
                 R"("m":"ORDER")"
                 R"(})"
                 R"(})";
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ACCOUNT_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
