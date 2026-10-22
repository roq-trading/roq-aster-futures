/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/protocol/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/utils/hash/fnv.hpp"

#include "roq/core/json/parser.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace protocol {
namespace json {

// === CONSTANTS ===

namespace {
constexpr auto const KEY_EVENT_TYPE = "e"sv;
constexpr auto const KEY_ID = "id"sv;
}  // namespace

// === HELPERS ===

namespace {
template <typename T>
auto dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj);
  return true;
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  EventType event_type;
  auto ack = false;
  auto helper = [&](auto &key, auto &value) {
    auto key_2 = utils::hash::FNV::compute(key);
    switch (key_2) {
      case utils::hash::FNV::compute(KEY_EVENT_TYPE):
        new (&event_type) EventType{value};
        break;
      case utils::hash::FNV::compute(KEY_ID):
        ack = true;
        break;
    }
    return true;
  };
  core::json::Parser::dispatch<core::json::Object>(helper, message);
  if (ack) {
    return dispatch_helper<Ack>(handler, message, buffer_stack, trace_info);
  }
  switch (event_type) {
    using enum EventType::type_t;
    case UNDEFINED_INTERNAL:
      log::fatal("Unexpected"sv);
    case UNKNOWN_INTERNAL:
      break;
    case ANNOUNCEMENT:
      return false;  // note! will be logged
    // market data
    case AGG_TRADE:
      return dispatch_helper<AggTrade>(handler, message, buffer_stack, trace_info);
    case MARK_PRICE_UPDATE:
      return dispatch_helper<MarkPriceUpdate>(handler, message, buffer_stack, trace_info);
    case _24HR_MINI_TICKER:
      return dispatch_helper<MiniTicker>(handler, message, buffer_stack, trace_info);
    case _24HR_TICKER:
      return dispatch_helper<Ticker>(handler, message, buffer_stack, trace_info);
    case BOOK_TICKER:
      return dispatch_helper<BookTicker>(handler, message, buffer_stack, trace_info);
    case DEPTH_UPDATE:
      return dispatch_helper<DepthUpdate>(handler, message, buffer_stack, trace_info);
    case FORCE_ORDER:
      return dispatch_helper<ForceOrder>(handler, message, buffer_stack, trace_info);
    // order management
    case LISTEN_KEY_EXPIRED:
      return dispatch_helper<ListenKeyExpired>(handler, message, buffer_stack, trace_info);
    case MARGIN_CALL:
      log::warn("DEBUG {}"sv, message);
      return dispatch_helper<MarginCall>(handler, message, buffer_stack, trace_info);
    case ACCOUNT_UPDATE:
      log::warn("DEBUG {}"sv, message);
      return dispatch_helper<AccountUpdate>(handler, message, buffer_stack, trace_info);
    case ORDER_TRADE_UPDATE:
      return dispatch_helper<OrderTradeUpdate>(handler, message, buffer_stack, trace_info);
    case ACCOUNT_CONFIG_UPDATE:
      log::warn("DEBUG {}"sv, message);
      return dispatch_helper<AccountConfigUpdate>(handler, message, buffer_stack, trace_info);
  }
  if (allow_unknown_event_types) {
    return false;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace protocol
}  // namespace aster_futures
}  // namespace roq
