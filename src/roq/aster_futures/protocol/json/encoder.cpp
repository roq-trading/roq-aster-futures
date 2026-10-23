/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/protocol/json/encoder.hpp"

#include <fmt/format.h>

#include "roq/decimal.hpp"

#include "roq/aster_futures/protocol/json/map.hpp"
#include "roq/aster_futures/protocol/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace protocol {
namespace json {

std::string_view Encoder::listen_key(std::string &buffer) {
  buffer.clear();
  return buffer;
}

std::string_view Encoder::account(std::string &buffer) {
  buffer.clear();
  return buffer;
}

std::string_view Encoder::open_orders(std::string &buffer, std::string_view const &symbol) {
  buffer.clear();
  if (!std::empty(symbol)) {
    fmt::format_to(std::back_inserter(buffer), "symbol={}"sv, symbol);
  }
  return buffer;
}

std::string_view Encoder::user_trades(std::string &buffer, std::chrono::milliseconds start_time, std::string_view const &symbol) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      "startTime={}"
      "&symbol={}"sv,
      start_time.count(),
      symbol);
  return buffer;
}

std::string_view Encoder::place_order(
    std::string &buffer,
    CreateOrder const &create_order,
    server::oms::Order const &,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id) {
  buffer.clear();
  auto reduce_only = create_order.execution_instructions.has(ExecutionInstruction::DO_NOT_INCREASE);
  fmt::format_to(
      std::back_inserter(buffer),
      "newClientOrderId={}"
      "&symbol={}"
      "&type={}"
      "&side={}"
      "&reduceOnly={}"
      "&quantity={}"sv,
      request_id,
      create_order.symbol,
      map(create_order.order_type).template get<json::OrderType>().as_raw_text(),
      map(create_order.side).template get<json::Side>().as_raw_text(),
      reduce_only,
      Decimal{create_order.quantity, ref_data.quantity.precision});
  if (create_order.order_type == roq::OrderType::LIMIT) {
    auto time_in_force = [&]() -> json::TimeInForce {
      if (create_order.execution_instructions.has(ExecutionInstruction::PARTICIPATE_DO_NOT_INITIATE)) {
        return json::TimeInForce::GTX;
      }
      return map(create_order.time_in_force);
    }();
    fmt::format_to(
        std::back_inserter(buffer),
        "&price={}"
        "&timeInForce={}"sv,
        Decimal{create_order.price, ref_data.price.precision},
        time_in_force.as_raw_text());
  }
  if (create_order.position_effect != roq::PositionEffect{}) {
    fmt::format_to(
        std::back_inserter(buffer),
        "&positionSide={}"sv,
        map(create_order.position_effect, create_order.side).template get<json::PositionSide>().as_raw_text());
  }
  return buffer;
}

std::string_view Encoder::modify_order(
    std::string &buffer,
    ModifyOrder const &modify_order,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "symbol={}"sv, order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "&orderId={}"sv, order.external_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "&origClientOrderId={}"sv, order.client_order_id);
  }
  auto quantity = [&]() {
    if (std::isnan(modify_order.quantity)) {
      return order.quantity;
    }
    return modify_order.quantity;
  }();
  auto price = [&]() {
    if (std::isnan(modify_order.price)) {
      return order.price;
    }
    return modify_order.price;
  }();
  fmt::format_to(
      std::back_inserter(buffer),
      "&quantity={}"
      "&price={}"sv,
      Decimal{quantity, ref_data.quantity.precision},
      Decimal{price, ref_data.price.precision});
  return buffer;
}

std::string_view Encoder::cancel_order(
    std::string &buffer,
    CancelOrder const &,
    server::oms::Order const &order,
    server::oms::RefData const &,
    [[maybe_unused]] std::string_view const &request_id) {
  buffer.clear();
  fmt::format_to(std::back_inserter(buffer), "symbol={}"sv, order.symbol);
  if (std::empty(order.external_order_id)) {
    fmt::format_to(std::back_inserter(buffer), "&orderId={}"sv, order.external_order_id);
  } else {
    fmt::format_to(std::back_inserter(buffer), "&origClientOrderId={}"sv, order.client_order_id);
  }
  return buffer;
}

std::string_view Encoder::cancel_all_orders(
    std::string &buffer, CancelAllOrders const &cancel_all_orders, [[maybe_unused]] std::string_view const &request_id, std::string_view const &symbol) {
  buffer.clear();
  auto symbol_2 = [&]() {
    if (std::empty(cancel_all_orders.symbol)) {
      return symbol;
    }
    return cancel_all_orders.symbol;
  }();
  fmt::format_to(std::back_inserter(buffer), "symbol={}"sv, symbol_2);
  return buffer;
}

std::string_view Encoder::countdown_cancel_all(std::string &buffer, std::chrono::milliseconds countdown, std::string_view const &symbol) {
  buffer.clear();
  fmt::format_to(
      std::back_inserter(buffer),
      "countdown={}"
      "&symbol={}"sv,
      countdown.count(),
      symbol);
  return buffer;
}

}  // namespace json
}  // namespace protocol
}  // namespace aster_futures
}  // namespace roq
