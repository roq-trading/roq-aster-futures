/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>

#include "roq/utils/metrics/counter.hpp"
#include "roq/utils/metrics/latency.hpp"
#include "roq/utils/metrics/profile.hpp"

#include "roq/io/context.hpp"

#include "roq/web/socket/client.hpp"

#include "roq/core/download_2.hpp"

#include "roq/core/json/buffer_stack.hpp"

#include "roq/server/stream.hpp"

#include "roq/aster_futures/gateway/account.hpp"
#include "roq/aster_futures/gateway/request.hpp"
#include "roq/aster_futures/gateway/shared.hpp"

#include "roq/aster_futures/protocol/json/parser.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct DropCopy final : public Base<DropCopy>, public server::Stream, public web::socket::Client::Handler, protocol::json::Parser::Handler {
  struct Handler {};

  DropCopy(Handler &, io::Context &, uint16_t stream_id, Account &, Shared &, Request &, std::string_view const &listen_key);

  // protected:
  friend base_type;

  // server::Stream

  uint16_t stream_id() const override { return stream_id_; }

  bool ready() const override;

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(metrics::Writer &) const override;

  void operator()(Trace<ConnectionStatus> const &, std::string_view const &reason = {}) override;

 protected:
  // web::socket::Client::Handler

  void operator()(Trace<web::socket::Connected> const &) override;
  void operator()(Trace<web::socket::Disconnected> const &) override;
  void operator()(Trace<web::socket::Ready> const &) override;
  void operator()(Trace<web::socket::Close> const &) override;
  void operator()(Trace<web::socket::Latency> const &) override;
  void operator()(Trace<web::socket::Text> const &) override;
  void operator()(Trace<web::socket::Binary> const &) override;

  // core::Download

  enum class State {
    UNDEFINED = 0,
    ACCOUNT,
    OPEN_ORDERS,
    USER_TRADES,
    DONE,
  };

  int32_t download(Trace<State> const &);

  // protocol::json::Parser::Handler

  void operator()(Trace<protocol::json::Pong> const &) override;
  void operator()(Trace<protocol::json::Ack> const &) override;

  void operator()(Trace<protocol::json::AggTrade> const &) override;
  void operator()(Trace<protocol::json::MarkPriceUpdate> const &) override;
  void operator()(Trace<protocol::json::MiniTicker> const &) override;
  void operator()(Trace<protocol::json::Ticker> const &) override;
  void operator()(Trace<protocol::json::BookTicker> const &) override;
  void operator()(Trace<protocol::json::DepthUpdate> const &) override;

  void operator()(Trace<protocol::json::OrderTradeUpdate> const &) override;

  // helpers

  void check_response(TraceInfo const &);

  void parse(TraceInfo const &, std::string_view const &message);

 private:
  [[maybe_unused]] Handler &handler_;
  // config
  uint16_t const stream_id_;
  std::string const name_;
  // web socket
  std::unique_ptr<web::socket::Client> connection_;
  // buffers
  core::json::BufferStack decode_buffer_;
  // metrics
  struct {
    utils::metrics::Counter disconnect;
  } counter_;
  struct {
    utils::metrics::Profile parse;
  } profile_;
  struct {
    utils::metrics::Latency ping, heartbeat;
  } latency_;
  // account
  Account &account_;
  // cache
  Shared &shared_;
  Request &request_;
  // state
  ConnectionStatus connection_status_ = {};
  core::Download2<State> download_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
