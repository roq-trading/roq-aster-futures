/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/drop_copy.hpp"

#include "roq/mask.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/aster_futures/protocol/json/map.hpp"
#include "roq/aster_futures/protocol/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const NAME = "ex"sv;

auto const SUPPORTS = Mask{
    SupportType::ORDER,
    SupportType::TRADE,
    SupportType::FUNDS,
    SupportType::POSITION,
};

auto const PING = "ping"sv;

size_t const MAX_DECODE_BUFFER_DEPTH = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_uri(auto &settings, auto &listen_key) {
  auto &uri = settings.ws.uri;
  auto result = fmt::format("{}://{}{}/{}"sv, uri.get_scheme(), uri.get_host(), uri.get_path(), listen_key);
  return io::web::URI{result};
}

auto create_connection(auto &handler, auto &settings, auto &context, auto &shared, auto &listen_key) {
  auto uri = create_uri(settings, listen_key);
  auto config = web::socket::Client::Config{
      // connection
      .interface = {},
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = settings.net.connection_timeout,
      .disconnect_on_idle_timeout = {},
      .always_reconnect = true,
      // proxy
      .proxy = {},
      // http
      .user_agent = ROQ_PACKAGE_NAME,
      .request_timeout = {},
      .ping_frequency = settings.ws.ping_freq,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::socket::Client::create(handler, context, config, shared.throttle, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

DropCopy::DropCopy(
    Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, Request &request, std::string_view const &listen_key)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)},
      connection_{create_connection(*this, shared.settings, context, shared, listen_key)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
          .heartbeat = create_metrics(shared.settings, name_, "heartbeat"sv),
      },
      account_{account}, shared_{shared}, request_{request}, download_{shared.settings.rest.request_timeout, [this](auto &event) { return download(event); }} {
}

// server::Stream

bool DropCopy::ready() const {
  return (*connection_).ready();
}

void DropCopy::operator()(Trace<Start> const &) {
  (*connection_).start();
}

void DropCopy::operator()(Trace<Stop> const &) {
  (*connection_).stop();
}

void DropCopy::operator()(Trace<Timer> const &event) {
  auto &[trace_info, timer] = event;
  if ((*connection_).refresh(timer.now)) {
    if (ready()) {
      check_response(trace_info);
    }
  }
}

void DropCopy::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY)
      .write(latency_.heartbeat, metrics::Type::LATENCY);
}

void DropCopy::operator()(Trace<ConnectionStatus> const &event, std::string_view const &reason) {
  auto &[trace_info, connection_status] = event;
  connection_status_ = connection_status;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = account_.name,
      .supports = SUPPORTS,
      .transport = Transport::TCP,
      .protocol = Protocol::WS,
      .encoding = {Encoding::JSON},
      .priority = Priority::PRIMARY,
      .connection_status = connection_status_,
      .reason = reason,
      .interface = (*connection_).get_interface(),
      .authority = (*connection_).get_current_authority(),
      .path = (*connection_).get_current_path(),
      .proxy = (*connection_).get_proxy(),
  };
  log::info("stream_status={}"sv, stream_status);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, stream_status);
}

// web::socket::Client::Handler

void DropCopy::operator()(Trace<web::socket::Connected> const &) {
}

void DropCopy::operator()(Trace<web::socket::Disconnected> const &event) {
  auto &[trace_info, disconnected] = event;
  ++counter_.disconnect;
  create_trace_and_dispatch_2(trace_info, ConnectionStatus::DISCONNECTED);
  download_.reset();
}

void DropCopy::operator()(Trace<web::socket::Ready> const &event) {
  auto &[trace_info, ready] = event;
  log::warn("DEBUG READY"sv);
  download_.begin(trace_info);
}

void DropCopy::operator()(Trace<web::socket::Close> const &event) {
  auto &[trace_info, close] = event;
  log::warn("close={}"sv, close);
}

void DropCopy::operator()(Trace<web::socket::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(shared_.dispatcher, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void DropCopy::operator()(Trace<web::socket::Text> const &event) {
  auto &[trace_info, text] = event;
  parse(trace_info, text.payload);
}

void DropCopy::operator()(Trace<web::socket::Binary> const &) {
  log::fatal("Unexpected"sv);
}

// core::Download

int32_t DropCopy::download(Trace<State> const &event) {
  auto &[trace_info, state] = event;
  log::warn("DEBUG {}"sv, state);
  switch (state) {
    using enum State;
    case UNDEFINED:
      assert(false);
      break;
    case ACCOUNT:
      create_trace_and_dispatch_2(trace_info, ConnectionStatus::DOWNLOADING, "account"sv);
      request_.account.request(trace_info);
      return 1;
    case OPEN_ORDERS:
      create_trace_and_dispatch_2(trace_info, ConnectionStatus::DOWNLOADING, "open-orders"sv);
      request_.open_orders.request(trace_info);
      return 1;
    case USER_TRADES:
      create_trace_and_dispatch_2(trace_info, ConnectionStatus::DOWNLOADING, "user-trades"sv);
      return 1;
    case DONE:
      create_trace_and_dispatch_2(trace_info, ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// protocol::json::Parser::Handler

void DropCopy::operator()(Trace<protocol::json::Pong> const &event) {
  auto &[trace_info, pong] = event;
  log::warn("DEBUG pong={}"sv, pong);
}

// - market data

void DropCopy::operator()(Trace<protocol::json::Ack> const &event) {
  auto &[trace_info, ack] = event;
  log::warn("DEBUG ack={}"sv, ack);
}

void DropCopy::operator()(Trace<protocol::json::AggTrade> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::MarkPriceUpdate> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::MiniTicker> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::Ticker> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::BookTicker> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::DepthUpdate> const &) {
  log::fatal("Unexpected"sv);
}

void DropCopy::operator()(Trace<protocol::json::ForceOrder> const &) {
  log::fatal("Unexpected"sv);
}

// - order management

void DropCopy::operator()(Trace<protocol::json::ListenKeyExpired> const &event) {
  auto &[trace_info, listen_key_expired] = event;
}

void DropCopy::operator()(Trace<protocol::json::MarginCall> const &event) {
  auto &[trace_info, margin_call] = event;
}

void DropCopy::operator()(Trace<protocol::json::AccountUpdate> const &event) {
  auto &[trace_info, account_update] = event;
}

void DropCopy::operator()(Trace<protocol::json::OrderTradeUpdate> const &event) {
  auto &[trace_info, order_trade_update] = event;
  log::info<2>("order_trade_update={}"sv, order_trade_update);
  log::warn("DEBUG order_trade_update={}"sv, order_trade_update);
  auto &order = order_trade_update.order;
  auto external_order_id = fmt::format("{}"sv, order.order_id);
  auto remaining_quantity = order.original_quantity - order.accumulated_quantity;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = order.symbol,
      .side = map(order.side),
      .position_effect = map(order.position_side, order.side),
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(order.order_type),
      .time_in_force = map(order.time_in_force),
      .execution_instructions = {},  // XXX order_type + reduce_only
      .execution_destination = {},
      .create_time_utc = {},
      .update_time_utc = order.order_trade_time,
      .external_account = {},
      .external_order_id = external_order_id,
      .client_order_id = order.client_order_id,
      .order_status = map(order.order_status),
      .error = {},
      .text = {},
      .quantity = order.original_quantity,
      .price = order.original_price,
      .stop_price = NaN,
      .leverage = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = order.accumulated_quantity,
      .average_traded_price = order.average_price,
      .last_traded_quantity = order.last_filled_quantity,
      .last_traded_price = order.last_filled_price,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = order_trade_update.event_time,
  };
  log::warn("DEBUG order_update={}"sv, order_update);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
}

void DropCopy::operator()(Trace<protocol::json::AccountConfigUpdate> const &event) {
  auto &[trace_info, account_config_update] = event;
}

// helpers

void DropCopy::check_response(TraceInfo const &trace_info) {
  switch (download_.state()) {
    using enum State;
    case UNDEFINED:
      break;
    case ACCOUNT:
      if (request_.account.has_response()) {
        download_.check(trace_info, State::ACCOUNT);
      }
      break;
    case OPEN_ORDERS:
      if (request_.open_orders.has_response()) {
        download_.check(trace_info, State::OPEN_ORDERS);
      }
      break;
    case USER_TRADES:
      if (request_.user_trades.has_response()) {
        download_.check(trace_info, State::USER_TRADES);
      }
      break;
    case DONE:
      break;
  }
}

void DropCopy::parse(TraceInfo const &trace_info, std::string_view const &message) {
  profile_.parse([&]() {
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      if (!protocol::json::Parser::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
