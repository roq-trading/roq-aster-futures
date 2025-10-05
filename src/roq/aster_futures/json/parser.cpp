/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/aster_futures/json/parser.hpp"

#include "roq/logging.hpp"

#include "roq/core/json/parser.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace json {

// === CONSTANTS ===

namespace {
auto const EVENT_TYPE = "e"sv;
auto const ID = "id"sv;
}  // namespace

// === HELPERS ===

namespace {
template <typename T>
void dispatch_helper(auto &handler, auto &message, auto &buffer_stack, auto &trace_info) {
  T obj{message, buffer_stack};
  create_trace_and_dispatch(handler, trace_info, obj);
}
}  // namespace

// === IMPLEMENTATION ===

bool Parser::dispatch(
    Handler &handler, std::string_view const &message, core::json::BufferStack &buffer_stack, TraceInfo const &trace_info, bool allow_unknown_event_types) {
  auto ack = false;
  EventType event_type = {};
  core::json::Parser parser{message};
  auto value = parser.root();
  // XXX FIXME TODO stop early
  auto helper = [&](auto &key, auto &value_2) {
    if (key == EVENT_TYPE) {
      new (&event_type) EventType{value_2};
    } else if (key == ID) {
      ack = true;
    }
  };
  std::get<core::json::Object>(value).dispatch(helper);
  if (ack) {
    return true;
  }
  switch (event_type) {
    using enum EventType::type_t;
    case UNDEFINED_INTERNAL:
      assert(false);
    case UNKNOWN_INTERNAL:
      if (allow_unknown_event_types) {
        return false;
      }
      break;
    case AGG_TRADE:
      dispatch_helper<AggTrade>(handler, message, buffer_stack, trace_info);
      return true;
    case MARK_PRICE_UPDATE:
      dispatch_helper<MarkPriceUpdate>(handler, message, buffer_stack, trace_info);
      return true;
    case _24HR_MINI_TICKER:
      dispatch_helper<MiniTicker>(handler, message, buffer_stack, trace_info);
      return true;
    case _24HR_TICKER:
      dispatch_helper<Ticker>(handler, message, buffer_stack, trace_info);
      return true;
    case BOOK_TICKER:
      dispatch_helper<BookTicker>(handler, message, buffer_stack, trace_info);
      return true;
    case DEPTH_UPDATE:
      dispatch_helper<DepthUpdate>(handler, message, buffer_stack, trace_info);
      return true;
  }
  if (allow_unknown_event_types) {
    return false;
  }
  log::fatal(R"(Unexpected: message="{}")"sv, message);
}

}  // namespace json
}  // namespace aster_futures
}  // namespace roq
