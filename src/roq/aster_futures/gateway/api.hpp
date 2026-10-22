/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string_view>

#include "roq/aster_futures/gateway/settings.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct API final {
  struct {
    std::string_view exchange_info;
    std::string_view depth;
  } market_data;

  struct {
    std::string_view listen_key;
    std::string_view account;
    std::string_view open_orders;
    std::string_view user_trades;
    std::string_view place_order;
    std::string_view modify_order;
    std::string_view cancel_order;
    std::string_view cancel_all_orders;
    std::string_view countdown_cancel_all;
  } order_management;

  // factory
  static API create(Settings const &);
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
