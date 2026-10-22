/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "parser_tester.hpp"

using namespace roq;
using namespace roq::aster_futures;

using namespace std::literals;

using value_type = protocol::json::AccountConfigUpdate;

TEST_CASE("simple_1", "[json_account_config_update]") {
  auto message = R"({)"
                 R"("e":"ACCOUNT_CONFIG_UPDATE",)"
                 R"("E":1611646737479,)"
                 R"("T":1611646737476,)"
                 R"("ac":{)"
                 R"("s":"BTCUSDT",)"
                 R"("l":25)"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ACCOUNT_CONFIG_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}

TEST_CASE("simple_2", "[json_account_config_update]") {
  auto message = R"({)"
                 R"("e":"ACCOUNT_CONFIG_UPDATE",)"
                 R"("E":1611646737479,)"
                 R"("T":1611646737476,)"
                 R"("ai":{)"
                 R"("j":true,)"
                 R"("f":true,)"
                 R"("d":true)"
                 R"(})"
                 R"(})"sv;
  auto helper = [](value_type const &obj) { CHECK(obj.event_type == protocol::json::EventType::ACCOUNT_CONFIG_UPDATE); };
  ParserTester<value_type>::dispatch(helper, message, 8192, 2);
}
