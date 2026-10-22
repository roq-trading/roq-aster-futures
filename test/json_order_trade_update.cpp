/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::OrderTradeUpdate;

TEST_CASE("new", "[json_order_trade_update]") {
  auto message = R"({)"
                 R"("e":"ORDER_TRADE_UPDATE",)"
                 R"("T":1792580554000,)"
                 R"("E":1792580554033,)"
                 R"("o":{)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"wgAC2gmkdYQAAQAAAAAA",)"
                 R"("S":"BUY",)"
                 R"("o":"LIMIT",)"
                 R"("f":"GTC",)"
                 R"("q":"0.001",)"
                 R"("p":"50000",)"
                 R"("ap":"0",)"
                 R"("sp":"0",)"
                 R"("x":"NEW",)"
                 R"("X":"NEW",)"
                 R"("i":33745693585,)"
                 R"("l":"0",)"
                 R"("z":"0",)"
                 R"("L":"0",)"
                 R"("T":1792580554000,)"
                 R"("t":0,)"
                 R"("b":"50",)"
                 R"("a":"0",)"
                 R"("m":false,)"
                 R"("R":false,)"
                 R"("wt":"CONTRACT_PRICE",)"
                 R"("ot":"LIMIT",)"
                 R"("ps":"BOTH",)"
                 R"("cp":false,)"
                 R"("rp":"0",)"
                 R"("pP":false,)"
                 R"("si":0,)"
                 R"("ss":0,)"
                 R"("h":"0x86eb980d0643069b42d18e6dff0adde49ff53387448beda9359e748e05379983")"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ORDER_TRADE_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}

TEST_CASE("trade_filled", "[json_order_trade_update]") {
  auto message = R"({)"
                 R"("e":"ORDER_TRADE_UPDATE",)"
                 R"("T":1792673435100,)"
                 R"("E":1792673435136,)"
                 R"("o":{)"
                 R"("s":"BTCUSDT",)"
                 R"("c":"6gACZeIArYQAAQAAAAAA",)"
                 R"("S":"BUY",)"
                 R"("o":"MARKET",)"
                 R"("f":"GTC",)"
                 R"("q":"0.001",)"
                 R"("p":"0",)"
                 R"("ap":"0",)"
                 R"("sp":"0",)"
                 R"("x":"NEW",)"
                 R"("X":"NEW",)"
                 R"("i":33770620387,)"
                 R"("l":"0",)"
                 R"("z":"0",)"
                 R"("L":"0",)"
                 R"("T":1792673435100,)"
                 R"("t":0,)"
                 R"("b":"0",)"
                 R"("a":"0",)"
                 R"("m":false,)"
                 R"("R":false,)"
                 R"("wt":"CONTRACT_PRICE",)"
                 R"("ot":"MARKET",)"
                 R"("ps":"BOTH",)"
                 R"("cp":false,)"
                 R"("rp":"0",)"
                 R"("pP":false,)"
                 R"("si":0,)"
                 R"("ss":0,)"
                 R"("h":"0x2fe33a5c5e3521db08bd7c5d295f5244692de0e4e91deed5dd9d3b64b41526a7")"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ORDER_TRADE_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 1);
}
