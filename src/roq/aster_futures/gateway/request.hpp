/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/core/limit/request.hpp"

#include "roq/aster_futures/gateway/shared.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct Request final : public core::limit::Request::Handler {
  explicit Request(Shared &);

  // core::Request::Handler

  bool ready() const override;
  bool can_request(TraceInfo const &) override;

  // ...

  core::limit::Request listen_key;
  core::limit::Request account;
  core::limit::Request open_orders;
  core::limit::Request user_trades;

 private:
  Shared &shared_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
