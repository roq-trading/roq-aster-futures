/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <chrono>
#include <string>
#include <string_view>

#include "roq/cancel_all_orders.hpp"
#include "roq/cancel_order.hpp"
#include "roq/create_order.hpp"
#include "roq/modify_order.hpp"

#include "roq/server/oms/order.hpp"
#include "roq/server/oms/ref_data.hpp"

namespace roq {
namespace aster_futures {
namespace protocol {
namespace json {

struct Encoder final {
  static std::string_view listen_key(std::string &buffer);

  static std::string_view account(std::string &buffer);

  static std::string_view open_orders(std::string &buffer, std::string_view const &symbol = {});

  static std::string_view user_trades(std::string &buffer, std::chrono::milliseconds start_time, std::string_view const &symbol);

  static std::string_view place_order(
      std::string &buffer, CreateOrder const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);

  static std::string_view modify_order(
      std::string &buffer, ModifyOrder const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);

  static std::string_view cancel_order(
      std::string &buffer, CancelOrder const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);

  static std::string_view cancel_all_orders(
      std::string &buffer, CancelAllOrders const &, std::string_view const &request_id, std::string_view const &symbol = {});

  static std::string_view countdown_cancel_all(std::string &buffer, std::chrono::milliseconds countdown, std::string_view const &symbol);
};

}  // namespace json
}  // namespace protocol
}  // namespace aster_futures
}  // namespace roq
