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
      .market_data{
          .exchange_info = "/fapi/v3/exchangeInfo"sv,
          .depth = "/fapi/v3/depth"sv,
      },
      .order_management{
          .listen_key = "/fapi/v3/listenKey"sv,
          .account = "/fapi/v3/accountWithJoinMargin"sv,
          .open_orders = "/fapi/v3/openOrders"sv,
          .user_trades = "/fapi/v3/userTrades"sv,
          .place_order = "/fapi/v3/order"sv,
          .modify_order = "/fapi/v3/order"sv,
          .cancel_order = "/fapi/v3/order"sv,
          .cancel_all_orders = "/fapi/v3/allOpenOrders"sv,
          .countdown_cancel_all = "/fapi/v3/countdownCancelAll"sv,
      },
  };
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
