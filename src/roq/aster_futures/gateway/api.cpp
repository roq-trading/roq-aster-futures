/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/api.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === IMPLEMENTATION ===

API API::create(Settings const &) {
  return {
      .category = {},
      .inst_type = {},
      .market_data{
          .exchange_info = "/fapi/v3/exchangeInfo"sv,
          .depth = "/fapi/v3/depth"sv,
      },
      .order_management{
          .account_info = "/fapi/v3/balance"sv,
          .account_assets = "/fapi/v3/accountWithJoinMargin"sv,
          .position_info = "/fapi/v3/positionRisk"sv,
          .open_orders = "/fapi/v3/openOrders"sv,
          .fill_history = "/fapi/v3/userTrades"sv,
          .place_order = "/fapi/v3/order"sv,
          .modify_order = "/fapi/v3/order"sv,
          .cancel_order = "/fapi/v3/order"sv,
          .cancel_all_orders = "/fapi/v3/allOpenOrders"sv,
          .countdown_cancel_all = "xxx"sv,
      },
  };
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
