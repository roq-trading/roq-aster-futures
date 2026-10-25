/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/order_entry.hpp"

#include "roq/mask.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/server/oms/exceptions.hpp"

#include "roq/aster_futures/protocol/json/encoder.hpp"
#include "roq/aster_futures/protocol/json/map.hpp"
#include "roq/aster_futures/protocol/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const NAME = "om"sv;

auto const SUPPORTS = Mask{
    SupportType::CREATE_ORDER,
    SupportType::MODIFY_ORDER,
    SupportType::CANCEL_ORDER,
    SupportType::ORDER_ACK,
    SupportType::FUNDS,
    SupportType::POSITION,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id, auto &account) {
  return fmt::format("{}:{}:{}"sv, stream_id, NAME, account.name);
}

auto create_connection(auto &handler, auto &settings, auto &context, auto &shared) {
  auto uri = settings.rest.uri;
  auto config = web::rest::Client::Config{
      // connection
      .interface = {},
      .proxy = settings.rest.proxy,
      .uris = {&uri, 1},
      .host = {},
      .validate_certificate = settings.net.tls_validate_certificate,
      // connection manager
      .connection_timeout = {},
      .disconnect_on_idle_timeout = {},
      .connection = web::http::Connection::KEEP_ALIVE,
      // request
      .allow_pipelining = true,
      .request_timeout = settings.rest.request_timeout,
      // response
      .suspend_on_retry_after = {},
      // http
      .query = {},
      .user_agent = ROQ_PACKAGE_NAME,
      .ping_frequency = settings.rest.ping_freq,
      .ping_path = settings.rest.ping_path,
      // implementation
      .decode_buffer_size = settings.misc.decode_buffer_size,
      .encode_buffer_size = settings.misc.encode_buffer_size,
  };
  return web::rest::Client::create(handler, context, config, shared.throttle);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

OrderEntry::OrderEntry(Handler &handler, io::Context &context, uint16_t stream_id, Account &account, Shared &shared, Request &request, bool master)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_, account)}, master_{master},
      connection_{create_connection(*this, shared.settings, context, shared)}, decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .listen_key = create_metrics(shared.settings, name_, "listen_key"sv),
          .listen_key_ack = create_metrics(shared.settings, name_, "listen_key_ack"sv),
          .account = create_metrics(shared.settings, name_, "account"sv),
          .account_ack = create_metrics(shared.settings, name_, "account_ack"sv),
          .open_orders = create_metrics(shared.settings, name_, "open_orders"sv),
          .open_orders_ack = create_metrics(shared.settings, name_, "open_orders_ack"sv),
          .user_trades = create_metrics(shared.settings, name_, "user_trades"sv),
          .user_trades_ack = create_metrics(shared.settings, name_, "user_trades_ack"sv),
          .place_order = create_metrics(shared.settings, name_, "place_order"sv),
          .place_order_ack = create_metrics(shared.settings, name_, "place_order_ack"sv),
          .modify_order = create_metrics(shared.settings, name_, "modify_order"sv),
          .modify_order_ack = create_metrics(shared.settings, name_, "modify_order_ack"sv),
          .cancel_order = create_metrics(shared.settings, name_, "cancel_order"sv),
          .cancel_order_ack = create_metrics(shared.settings, name_, "cancel_order_ack"sv),
          .cancel_all_orders = create_metrics(shared.settings, name_, "cancel_all_orders"sv),
          .cancel_all_orders_ack = create_metrics(shared.settings, name_, "cancel_all_orders_ack"sv),
          .countdown_cancel_all = create_metrics(shared.settings, name_, "countdown_cancel_all"sv),
          .countdown_cancel_all_ack = create_metrics(shared.settings, name_, "countdown_cancel_all_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      account_{account}, shared_{shared}, request_{request} {
}

// server::Stream

void OrderEntry::operator()(Trace<Start> const &) {
  (*connection_).start();
}

void OrderEntry::operator()(Trace<Stop> const &) {
  (*connection_).stop();
}

void OrderEntry::operator()(Trace<Timer> const &event) {
  auto &[trace_info, timer] = event;
  if ((*connection_).refresh(timer.now)) {
    if (master_ && ready()) {
      check_request_queue(trace_info);
      check_countdown_cancel_all(trace_info);
    }
  }
}

void OrderEntry::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.listen_key, metrics::Type::PROFILE)
      .write(profile_.listen_key_ack, metrics::Type::PROFILE)
      .write(profile_.account, metrics::Type::PROFILE)
      .write(profile_.account_ack, metrics::Type::PROFILE)
      .write(profile_.open_orders, metrics::Type::PROFILE)
      .write(profile_.open_orders_ack, metrics::Type::PROFILE)
      .write(profile_.user_trades, metrics::Type::PROFILE)
      .write(profile_.user_trades_ack, metrics::Type::PROFILE)
      .write(profile_.place_order, metrics::Type::PROFILE)
      .write(profile_.place_order_ack, metrics::Type::PROFILE)
      .write(profile_.modify_order, metrics::Type::PROFILE)
      .write(profile_.modify_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_order, metrics::Type::PROFILE)
      .write(profile_.cancel_order_ack, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders, metrics::Type::PROFILE)
      .write(profile_.cancel_all_orders_ack, metrics::Type::PROFILE)
      .write(profile_.countdown_cancel_all, metrics::Type::PROFILE)
      .write(profile_.countdown_cancel_all_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void OrderEntry::operator()(Trace<ConnectionStatus> const &event, std::string_view const &reason) {
  auto &[trace_info, connection_status] = event;
  connection_status_ = connection_status;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = account_.name,
      .supports = SUPPORTS,
      .transport = Transport::TCP,
      .protocol = Protocol::HTTP,
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

// server::OrderActionStream

uint16_t OrderEntry::operator()(
    Event<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  place_order(event, order, ref_data, request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  modify_order(event, order, ref_data, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  cancel_order(event, order, ref_data, request_id, previous_request_id);
  return stream_id_;
}

uint16_t OrderEntry::operator()(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  cancel_all_orders(event, request_id);
  return stream_id_;
}

// web::rest::Client::Handler

void OrderEntry::operator()(Trace<web::rest::Connected> const &event) {
  auto &[trace_info, connected] = event;
  create_trace_and_dispatch_2(trace_info, ConnectionStatus::READY);
}

void OrderEntry::operator()(Trace<web::rest::Disconnected> const &event) {
  auto &[trace_info, disconnected] = event;
  ++counter_.disconnect;
  create_trace_and_dispatch_2(trace_info, ConnectionStatus::DISCONNECTED);
  next_heartbeat_ = {};
}

void OrderEntry::operator()(Trace<web::rest::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = account_.name,
      .latency = latency.sample,
  };
  create_trace_and_dispatch(shared_.dispatcher, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

// listen-key

void OrderEntry::get_listen_key(TraceInfo const &trace_info) {
  profile_.listen_key([&]() {
    auto query = protocol::json::Encoder::listen_key(encode_buffer_);
    auto query_2 = account_.create_query(trace_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = shared_.api.order_management.listen_key,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    log::warn("DEBUG request={}"sv, request);
    auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { get_listen_key_ack(event); };
    (*connection_)(request, callback, "listen_key"sv);
  });
}

void OrderEntry::get_listen_key_ack(Trace<web::rest::Response> const &event) {
  profile_.listen_key_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) {
      log::warn("DEBUG body={}"sv, body);
      protocol::json::ListenKeyAck listen_key_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, listen_key_ack);
    };
    process_response(event, handle_error, handle_success);
    request_.listen_key.commit(trace_info);
  });
}

void OrderEntry::operator()(Trace<protocol::json::ListenKeyAck> const &event) {
  auto &[trace_info, listen_key_ack] = event;
  log::info<4>("listen_key_ack={}"sv, listen_key_ack);
  auto listen_key_update = ListenKeyUpdate{
      .account = account_.name,
      .listen_key = listen_key_ack.listen_key,
  };
  create_trace_and_dispatch(handler_, trace_info, listen_key_update);
}

// account

void OrderEntry::get_account(TraceInfo const &trace_info) {
  profile_.account([&]() {
    auto query = protocol::json::Encoder::account(encode_buffer_);
    auto query_2 = account_.create_query(trace_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.order_management.account,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    log::warn("DEBUG request={}"sv, request);
    auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { get_account_ack(event); };
    (*connection_)(request, callback, "account"sv);
  });
}

void OrderEntry::get_account_ack(Trace<web::rest::Response> const &event) {
  profile_.account_ack([&]() {
    auto &[trace_info, response] = event;
    // log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::AccountAck account_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, account_ack);
    };
    process_response(event, handle_error, handle_success);
    request_.account.commit(trace_info);
  });
}

void OrderEntry::operator()(Trace<protocol::json::AccountAck> const &event) {
  auto &[trace_info, account_ack] = event;
  log::info<4>("account_ack={}"sv, account_ack);
  for (auto &item : account_ack.assets) {
    auto funds_update = FundsUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .currency = item.asset,
        .margin_mode = {},
        .balance = item.wallet_balance,
        .hold = NaN,
        .borrowed = NaN,
        .unrealized_pnl = NaN,
        .external_account = {},
        .update_type = UpdateType::SNAPSHOT,
        .exchange_time_utc = item.update_time,
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(shared_.dispatcher, trace_info, funds_update, true);
  }
  for (auto &item : account_ack.positions) {
    auto long_quantity = std::max(0.0, item.position_amt);
    auto short_quantity = std::max(0.0, -item.position_amt);
    auto position_update = PositionUpdate{
        .stream_id = stream_id_,
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .margin_mode = MarginMode::CROSS,  // XXX FIXME TODO
        .external_account = {},
        .long_quantity = long_quantity,
        .short_quantity = short_quantity,
        .update_type = UpdateType::SNAPSHOT,
        .exchange_time_utc = item.update_time,
        .sending_time_utc = {},
    };
    create_trace_and_dispatch(shared_.dispatcher, trace_info, position_update, true);
  }
}

// open-orders

void OrderEntry::get_open_orders(TraceInfo const &trace_info) {
  profile_.open_orders([&]() {
    auto query = protocol::json::Encoder::open_orders(encode_buffer_);
    auto query_2 = account_.create_query(trace_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.order_management.open_orders,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { get_open_orders_ack(event); };
    (*connection_)(request, callback, "open_orders"sv);
  });
}

void OrderEntry::get_open_orders_ack(Trace<web::rest::Response> const &event) {
  profile_.open_orders_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::OpenOrdersAck open_orders_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, open_orders_ack);
    };
    process_response(event, handle_error, handle_success);
    request_.open_orders.commit(trace_info);
  });
}

void OrderEntry::operator()(Trace<protocol::json::OpenOrdersAck> const &event) {
  auto &[trace_info, open_orders_ack] = event;
  log::info<4>("open_orders_ack={}"sv, open_orders_ack);
  for (auto &item : open_orders_ack.data) {
    log::warn("DEBUG item={}"sv, item);
    auto external_order_id = fmt::format("{}"sv, item.order_id);
    auto remaining_quantity = item.orig_qty - item.executed_qty;
    auto order_update = server::oms::OrderUpdate{
        .account = account_.name,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .side = map(item.side),
        .position_effect = map(item.position_side, item.side),
        .margin_mode = {},
        .max_show_quantity = NaN,
        .order_type = map(item.type),
        .time_in_force = map(item.time_in_force),
        .execution_instructions = {},  // XXX map from time_in_force + reduce_only
        .execution_destination = {},
        .create_time_utc = item.time,
        .update_time_utc = item.update_time,
        .external_account = {},
        .external_order_id = external_order_id,
        .client_order_id = item.client_order_id,
        .order_status = map(item.status),
        .error = {},
        .text = {},
        .quantity = item.orig_qty,
        .price = item.price,
        .stop_price = NaN,
        .leverage = NaN,
        .remaining_quantity = remaining_quantity,
        .traded_quantity = item.executed_qty,
        .average_traded_price = item.avg_price,
        .last_traded_quantity = NaN,
        .last_traded_price = NaN,
        .last_liquidity = {},
        .routing_id = {},
        .max_request_version = {},
        .max_response_version = {},
        .max_accepted_version = {},
        .update_type = UpdateType::SNAPSHOT,
        .sending_time_utc = {},
    };
    log::warn("DEBUG order_update={}"sv, order_update);
    create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
  }
}

// user-trades

void OrderEntry::get_user_trades(TraceInfo const &trace_info) {
  profile_.user_trades([&]() {
    assert(shared_.settings.rest.download_fills_begin.count() > 0);
    auto start_time = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.origin_create_time_utc - shared_.settings.rest.download_fills_begin);
    auto helper = [&](auto &symbol) {
      auto query = protocol::json::Encoder::user_trades(encode_buffer_, start_time, symbol);
      auto query_2 = account_.create_query(trace_info, query);
      auto request = web::rest::Request{
          .method = web::http::Method::GET,
          .path = shared_.api.order_management.user_trades,
          .query = query_2,
          .accept = web::http::Accept::APPLICATION_JSON,
          .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
          .headers = {},
          .body = {},
          .quality_of_service = {},
      };
      auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { get_user_trades_ack(event); };
      (*connection_)(request, callback, "user_trades"sv);
    };
    for (auto &symbol : shared_.settings.download.symbols) {
      helper(symbol);
    }
  });
}

void OrderEntry::get_user_trades_ack(Trace<web::rest::Response> const &event) {
  profile_.user_trades_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::UserTradesAck user_trades{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, user_trades);
    };
    process_response(event, handle_error, handle_success);
    request_.user_trades.commit(trace_info);
  });
}

void OrderEntry::operator()(Trace<protocol::json::UserTradesAck> const &event) {
  auto &[trace_info, user_trades] = event;
  for (auto &item : user_trades.data) {
    log::info<4>("item={}"sv, item);
  }
}

// place-order

void OrderEntry::place_order(
    Event<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  profile_.place_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, create_order] = event;
    auto query = protocol::json::Encoder::place_order(encode_buffer_, create_order, order, ref_data, request_id);
    auto query_2 = account_.create_query(message_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = shared_.api.order_management.place_order,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = create_order.order_id](auto &event, [[maybe_unused]] auto &request_id) {
      uint32_t version = 1;
      place_order_ack(event, user_id, order_id, version);
    };
    (*connection_)(request, callback, request_id);
  });
}

void OrderEntry::place_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.place_order_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CREATE_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id_, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::PlaceOrderAck place_order_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, place_order_ack, user_id, order_id, version);
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<protocol::json::PlaceOrderAck> const &event,
    [[maybe_unused]] uint8_t user_id,
    [[maybe_unused]] uint64_t order_id,
    [[maybe_unused]] uint32_t version) {
  auto &[trace_info, place_order_ack] = event;
  log::info<2>("place_order_ack={}"sv, place_order_ack);
  log::warn("DEBUG place_order_ack={}"sv, place_order_ack);
  auto external_order_id = fmt::format("{}"sv, place_order_ack.order_id);
  auto remaining_quantity = place_order_ack.orig_qty - place_order_ack.executed_qty;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = place_order_ack.symbol,
      .side = map(place_order_ack.side),
      .position_effect = map(place_order_ack.position_side, place_order_ack.side),
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(place_order_ack.type),
      .time_in_force = map(place_order_ack.time_in_force),
      .execution_instructions = {},  // XXX map from time_in_force + reduce_only
      .execution_destination = {},
      .create_time_utc = place_order_ack.update_time,
      .update_time_utc = place_order_ack.update_time,
      .external_account = {},
      .external_order_id = external_order_id,
      .client_order_id = place_order_ack.client_order_id,
      .order_status = map(place_order_ack.status),
      .error = {},
      .text = {},
      .quantity = place_order_ack.orig_qty,
      .price = place_order_ack.price,
      .stop_price = NaN,
      .leverage = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = place_order_ack.executed_qty,
      .average_traded_price = place_order_ack.avg_price,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = {},
  };
  log::warn("DEBUG order_update={}"sv, order_update);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
}

// modify-order

void OrderEntry::modify_order(
    Event<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.modify_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, modify_order] = event;
    auto query = protocol::json::Encoder::modify_order(encode_buffer_, modify_order, order, ref_data, request_id);
    auto query_2 = account_.create_query(message_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::PUT,
        .path = shared_.api.order_management.modify_order,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = modify_order.order_id, version = modify_order.version](
                        auto &event, [[maybe_unused]] auto &request_id) { modify_order_ack(event, user_id, order_id, version); };
    (*connection_)(request, callback, request_id);
  });
}

void OrderEntry::modify_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.modify_order_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::MODIFY_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id_, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::ModifyOrderAck modify_order_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, modify_order_ack, user_id, order_id, version);
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<protocol::json::ModifyOrderAck> const &event,
    [[maybe_unused]] uint8_t user_id,
    [[maybe_unused]] uint64_t order_id,
    [[maybe_unused]] uint32_t version) {
  auto &[trace_info, modify_order_ack] = event;
  log::info<2>("modify_order_ack={}"sv, modify_order_ack);
  log::warn("DEBUG modify_order_ack={}"sv, modify_order_ack);
  auto external_order_id = fmt::format("{}"sv, modify_order_ack.order_id);
  auto remaining_quantity = modify_order_ack.orig_qty - modify_order_ack.executed_qty;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = modify_order_ack.symbol,
      .side = map(modify_order_ack.side),
      .position_effect = map(modify_order_ack.position_side, modify_order_ack.side),
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(modify_order_ack.type),
      .time_in_force = map(modify_order_ack.time_in_force),
      .execution_instructions = {},  // XXX map from time_in_force + reduce_only
      .execution_destination = {},
      .create_time_utc = modify_order_ack.update_time,
      .update_time_utc = modify_order_ack.update_time,
      .external_account = {},
      .external_order_id = external_order_id,
      .client_order_id = modify_order_ack.client_order_id,
      .order_status = map(modify_order_ack.status),
      .error = {},
      .text = {},
      .quantity = modify_order_ack.orig_qty,
      .price = modify_order_ack.price,
      .stop_price = NaN,
      .leverage = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = modify_order_ack.executed_qty,
      .average_traded_price = modify_order_ack.avg_price,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = {},
  };
  log::warn("DEBUG order_update={}"sv, order_update);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
}

// cancel-order

void OrderEntry::cancel_order(
    Event<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    [[maybe_unused]] std::string_view const &previous_request_id) {
  profile_.cancel_order([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_order] = event;
    auto query = protocol::json::Encoder::cancel_order(encode_buffer_, cancel_order, order, ref_data, request_id);
    auto query_2 = account_.create_query(message_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::DELETE,
        .path = shared_.api.order_management.cancel_order,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this, user_id = message_info.source, order_id = cancel_order.order_id, version = cancel_order.version](
                        auto &event, [[maybe_unused]] auto &request_id) { cancel_order_ack(event, user_id, order_id, version); };
    (*connection_)(request, callback, request_id);
  });
}

void OrderEntry::cancel_order_ack(Trace<web::rest::Response> const &event, uint8_t user_id, uint64_t order_id, uint32_t version) {
  profile_.cancel_order_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto response = server::oms::Response{
          .request_type = RequestType::CANCEL_ORDER,
          .origin = origin,
          .request_status = status,
          .error = error,
          .text = text,
          .version = version,
          .request_id = {},
          .external_order_id = {},
          .client_order_id = {},
          .quantity = NaN,
          .price = NaN,
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, response, stream_id_, user_id, order_id);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::CancelOrderAck cancel_order_ack{body, decode_buffer_};
      create_trace_and_dispatch_2(trace_info, cancel_order_ack, user_id, order_id, version);
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(
    Trace<protocol::json::CancelOrderAck> const &event,
    [[maybe_unused]] uint8_t user_id,
    [[maybe_unused]] uint64_t order_id,
    [[maybe_unused]] uint32_t version) {
  auto &[trace_info, cancel_order_ack] = event;
  log::info<2>("cancel_order_ack={}"sv, cancel_order_ack);
  log::warn("DEBUG cancel_order_ack={}"sv, cancel_order_ack);
  auto external_order_id = fmt::format("{}"sv, cancel_order_ack.order_id);
  auto remaining_quantity = cancel_order_ack.orig_qty - cancel_order_ack.executed_qty;
  auto order_update = server::oms::OrderUpdate{
      .account = account_.name,
      .exchange = shared_.settings.exchange,
      .symbol = cancel_order_ack.symbol,
      .side = map(cancel_order_ack.side),
      .position_effect = map(cancel_order_ack.position_side, cancel_order_ack.side),
      .margin_mode = {},
      .max_show_quantity = NaN,
      .order_type = map(cancel_order_ack.type),
      .time_in_force = map(cancel_order_ack.time_in_force),
      .execution_instructions = {},  // XXX map from time_in_force + reduce_only
      .execution_destination = {},
      .create_time_utc = cancel_order_ack.update_time,
      .update_time_utc = cancel_order_ack.update_time,
      .external_account = {},
      .external_order_id = external_order_id,
      .client_order_id = cancel_order_ack.client_order_id,
      .order_status = map(cancel_order_ack.status),
      .error = {},
      .text = {},
      .quantity = cancel_order_ack.orig_qty,
      .price = cancel_order_ack.price,
      .stop_price = NaN,
      .leverage = NaN,
      .remaining_quantity = remaining_quantity,
      .traded_quantity = cancel_order_ack.executed_qty,
      .average_traded_price = cancel_order_ack.avg_price,
      .last_traded_quantity = NaN,
      .last_traded_price = NaN,
      .last_liquidity = {},
      .routing_id = {},
      .max_request_version = {},
      .max_response_version = {},
      .max_accepted_version = {},
      .update_type = UpdateType::INCREMENTAL,
      .sending_time_utc = {},
  };
  log::warn("DEBUG order_update={}"sv, order_update);
  create_trace_and_dispatch(shared_.dispatcher, trace_info, order_update, stream_id_);
}

// cancel-all-orders

void OrderEntry::cancel_all_orders(Event<CancelAllOrders> const &event, std::string_view const &request_id) {
  profile_.cancel_all_orders([&]() {
    if (!ready()) {
      throw server::oms::NotReady{"not ready"sv};
    }
    auto &[message_info, cancel_all_orders] = event;
    auto helper = [&](auto &symbol) {
      if (!std::empty(cancel_all_orders.symbol) && symbol != cancel_all_orders.symbol) {
        return;
      }
      auto query = protocol::json::Encoder::cancel_all_orders(encode_buffer_, cancel_all_orders, request_id, symbol);
      auto query_2 = account_.create_query(message_info, query);
      auto request = web::rest::Request{
          .method = web::http::Method::DELETE,
          .path = shared_.api.order_management.cancel_all_orders,
          .query = query_2,
          .accept = web::http::Accept::APPLICATION_JSON,
          .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
          .headers = {},
          .body = {},
          .quality_of_service = {},
      };
      auto callback = [this, user_id = message_info.source](auto &event, [[maybe_unused]] auto &request_id) { cancel_all_orders_ack(event, user_id); };
      (*connection_)(request, callback, request_id);
    };
    if (shared_.dispatcher.get_all_order_symbols(helper, account_.name)) {
    } else {
      log::warn("*** NOT POSSIBLE TO CANCEL ALL OPEN ORDERS (NO SYMBOLS) ***"sv);
    }
  });
}

void OrderEntry::cancel_all_orders_ack(Trace<web::rest::Response> const &event, [[maybe_unused]] uint8_t user_id) {
  profile_.cancel_all_orders_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(DEBUG origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      auto cancel_all_orders_ack = CancelAllOrdersAck{
          .stream_id = stream_id_,
          .account = account_.name,
          .order_id = {},
          .exchange = shared_.settings.exchange,
          .symbol = {},
          .side = {},
          .origin = Origin::EXCHANGE,
          .request_status = RequestStatus::REJECTED,
          .error = error,
          .text = text,
          .request_id = {},  // ???
          .external_account = {},
          .number_of_affected_orders = {},
          .round_trip_latency = {},
          .user = {},
          .strategy_id = {},
      };
      create_trace_and_dispatch(shared_.dispatcher, trace_info, cancel_all_orders_ack);
    };
    auto handle_success = [&](auto &body) {
      protocol::json::CancelAllOrdersAck cancel_all_orders_ack{body, decode_buffer_};
      if (cancel_all_orders_ack.code == 0) {
        create_trace_and_dispatch_2(trace_info, cancel_all_orders_ack, user_id);
      } else {
        handle_error(Origin::EXCHANGE, RequestStatus::REJECTED, protocol::json::guess_error(cancel_all_orders_ack.code), cancel_all_orders_ack.msg);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void OrderEntry::operator()(Trace<protocol::json::CancelAllOrdersAck> const &event, [[maybe_unused]] uint8_t user_id) {
  auto &[trace_info, cancel_all_orders_ack] = event;
  log::info<2>("cancel_all_orders_ack={}"sv, cancel_all_orders_ack);
  log::warn("DEBUG cancel_all_orders_ack={}"sv, cancel_all_orders_ack);
}

// countdown-cancel-all

void OrderEntry::countdown_cancel_all(TraceInfo const &trace_info) {
  profile_.countdown_cancel_all([&]() {
    auto query =
        protocol::json::Encoder::countdown_cancel_all(encode_buffer_, std::chrono::duration_cast<std::chrono::seconds>(shared_.settings.rest.ping_freq), {});
    auto query_2 = account_.create_query(trace_info, query);
    auto request = web::rest::Request{
        .method = web::http::Method::POST,
        .path = shared_.api.order_management.countdown_cancel_all,
        .query = query_2,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = web::http::ContentType::APPLICATION_X_WWW_FORM_URLENCODED,
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto callback = [this](auto &event, [[maybe_unused]] auto &request_id) { countdown_cancel_all_ack(event); };
    (*connection_)(request, callback, "countdown_cancel_all"sv);
  });
}

void OrderEntry::countdown_cancel_all_ack(Trace<web::rest::Response> const &event) {
  profile_.countdown_cancel_all_ack([&]() {
    auto &[trace_info, response] = event;
    log::warn("DEBUG response={}"sv, response);
    auto [status, category, body] = response.result();
    if (status != web::http::Status::OK) {
      log::warn(R"(DEBUG status={}, category={}, body="{}")"sv, status, category, body);
    }
  });
}

// helpers

void OrderEntry::check_request_queue(TraceInfo const &trace_info) {
  request_.listen_key.check_pending(trace_info, [this](auto &trace_info) {
    log::warn("Requesting listen-key..."sv);
    get_listen_key(trace_info);
  });
  request_.account.check_pending(trace_info, [this](auto &trace_info) {
    log::warn("Requesting account..."sv);
    get_account(trace_info);
  });
  request_.open_orders.check_pending(trace_info, [this](auto &trace_info) {
    log::warn("Requesting open-orders..."sv);
    get_open_orders(trace_info);
  });
  request_.user_trades.check_pending(trace_info, [this](auto &trace_info) {
    log::warn("Requesting user-trades..."sv);
    get_user_trades(trace_info);
  });
}

void OrderEntry::check_countdown_cancel_all(TraceInfo const &trace_info) {
  if (shared_.settings.rest.cancel_on_disconnect && next_heartbeat_ < trace_info.origin_create_time) {
    next_heartbeat_ = trace_info.origin_create_time + (shared_.settings.rest.ping_freq / 3);
    countdown_cancel_all(trace_info);
  }
}

void OrderEntry::process_response(Trace<web::rest::Response> const &event, auto error_handler, auto success_handler) {
  auto &[trace_info, response] = event;
  try {
    auto [status, category, body] = response.result();
    switch (category) {
      using enum web::http::Category;
      case UNKNOWN:
      case INFORMATIONAL_RESPONSE:
        response.expect(web::http::Status::OK);  // throws
        break;
      case SUCCESS:
        success_handler(body);
        break;
      case REDIRECTION:
        log::fatal("Unexpected: URL is being redirected"sv);
      case CLIENT_ERROR:
        switch (status) {
          using enum web::http::Status;
          case FORBIDDEN:            // 403
          case TOO_MANY_REQUESTS: {  // 429
            auto message = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, message);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default: {
            // protocol::json::Message error{body};
            // XXX HANS error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, protocol::json::guess_error(error.code), error.msg);
          }
        }
        break;
      case SERVER_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
    }
  } catch (server::oms::Exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(e.origin, e.status, e.error, e.what());
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
