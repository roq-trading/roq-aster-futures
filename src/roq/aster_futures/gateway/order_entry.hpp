/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/rest/client.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server.hpp"

#include "roq/server/stream.hpp"

#include "roq/aster_futures/gateway/account.hpp"
#include "roq/aster_futures/gateway/request.hpp"
#include "roq/aster_futures/gateway/shared.hpp"

#include "roq/aster_futures/gateway/listen_key_update.hpp"

#include "roq/aster_futures/protocol/json/listen_key_ack.hpp"

#include "roq/aster_futures/protocol/json/account_ack.hpp"
#include "roq/aster_futures/protocol/json/open_orders_ack.hpp"
#include "roq/aster_futures/protocol/json/user_trades_ack.hpp"

#include "roq/aster_futures/protocol/json/cancel_all_orders_ack.hpp"
#include "roq/aster_futures/protocol/json/cancel_order_ack.hpp"
#include "roq/aster_futures/protocol/json/modify_order_ack.hpp"
#include "roq/aster_futures/protocol/json/place_order_ack.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct OrderEntry final : public Base<OrderEntry>, public server::OrderActionStream, public web::rest::Client::Handler {
  struct Handler {
    virtual void operator()(Trace<ListenKeyUpdate> const &) = 0;
  };

  OrderEntry(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, Request &, bool master);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override { return connection_status_ == ConnectionStatus::READY; }

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

  // server::OrderActionStream

  uint16_t operator()(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  uint16_t operator()(Event<CancelAllOrders> const &, std::string_view const &request_id) override;

 protected:
  // web::rest::Client::Handler

  void operator()(Trace<web::rest::Connected> const &) override;
  void operator()(Trace<web::rest::Disconnected> const &) override;
  void operator()(Trace<web::rest::Latency> const &) override;

  // listen-key

  void get_listen_key(TraceInfo const &);
  void get_listen_key_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::ListenKeyAck> const &);

  // account

  void get_account(TraceInfo const &);
  void get_account_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::AccountAck> const &);

  // open-orders

  void get_open_orders(TraceInfo const &);
  void get_open_orders_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::OpenOrdersAck> const &);

  // user-trades

  void get_user_trades(TraceInfo const &);
  void get_user_trades_ack(Trace<web::rest::Response> const &);
  void operator()(Trace<protocol::json::UserTradesAck> const &);

  // place-order

  void place_order(Event<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id);
  void place_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::PlaceOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // modify-order

  void modify_order(
      Event<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void modify_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::ModifyOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-order

  void cancel_order(
      Event<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id);
  void cancel_order_ack(Trace<web::rest::Response> const &, uint8_t user_id, uint64_t order_id, uint32_t version);
  void operator()(Trace<protocol::json::CancelOrderAck> const &, uint8_t user_id, uint64_t order_id, uint32_t version);

  // cancel-all-orders

  void cancel_all_orders(Event<CancelAllOrders> const &, std::string_view const &request_id);
  void cancel_all_orders_ack(Trace<web::rest::Response> const &, uint8_t user_id);
  void operator()(Trace<protocol::json::CancelAllOrdersAck> const &, uint8_t user_id);

  // countdown-cancel-all

  void countdown_cancel_all(TraceInfo const &);
  void countdown_cancel_all_ack(Trace<web::rest::Response> const &);
  // void operator()(Trace<protocol::json::CancelAllOrdersAck> const &, uint8_t user_id);

  // helpers

  void check_request_queue(TraceInfo const &);

  void check_countdown_cancel_all(TraceInfo const &);

  void process_response(Trace<web::rest::Response> const &, auto error_handler, auto success_handler);

 private:
  [[maybe_unused]] Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  bool const master_;
  // connection
  std::unique_ptr<web::rest::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile listen_key, listen_key_ack,  //
        account, account_ack,                            //
        open_orders, open_orders_ack,                    //
        user_trades, user_trades_ack,                    //
        place_order, place_order_ack,                    //
        modify_order, modify_order_ack,                  //
        cancel_order, cancel_order_ack,                  //
        cancel_all_orders, cancel_all_orders_ack,        //
        countdown_cancel_all, countdown_cancel_all_ack;
  } profile_;
  struct {
    utils::metrics::Latency ping;
  } latency_;
  // account
  Account &account_;
  Shared &shared_;
  Request &request_;
  // state
  ConnectionStatus connection_status_ = {};
  //
  std::string encode_buffer_;
  //
  std::chrono::nanoseconds next_heartbeat_ = {};
  std::chrono::nanoseconds xxx_ = {};
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
