/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "roq/server.hpp"

#include "roq/io/context.hpp"

#include "roq/aster_futures/gateway/account.hpp"
#include "roq/aster_futures/gateway/config.hpp"
#include "roq/aster_futures/gateway/request.hpp"
#include "roq/aster_futures/gateway/settings.hpp"
#include "roq/aster_futures/gateway/shared.hpp"

#include "roq/aster_futures/gateway/drop_copy.hpp"
#include "roq/aster_futures/gateway/market_data.hpp"
#include "roq/aster_futures/gateway/order_entry.hpp"
#include "roq/aster_futures/gateway/rest.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct Controller final : public Base<Controller>,
                          public server::Handler,
                          public Rest::Handler,
                          public OrderEntry::Handler,
                          public DropCopy::Handler,
                          public MarketData::Handler {
  ROQ_PUBLIC static std::unique_ptr<server::Handler> create(server::Dispatcher &, Settings const &, Config const &, io::Context &);

  ROQ_PUBLIC static uint8_t parse_api(Settings const &);

  Controller(server::Dispatcher &, Settings const &, Config const &, io::Context &);

  Controller(Controller const &) = delete;

 protected:
  friend base_type;

  // server::Handler

  void operator()(Trace<Start> const &) override;
  void operator()(Trace<Stop> const &) override;
  void operator()(Trace<Timer> const &) override;

  void operator()(Trace<Connected> const &, uint8_t user_id) override;
  void operator()(Trace<Disconnected> const &, uint8_t user_id) override;

  void operator()(Trace<Subscribe> const &) override;
  void operator()(Trace<Control> const &) override;

  void operator()(Trace<server::oms::OrderActionBatchBegin> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchCommit> const &) override;
  void operator()(Trace<server::oms::OrderActionBatchRollback> const &) override;

  uint16_t operator()(Trace<CreateOrder> const &, server::oms::Order const &, server::oms::RefData const &, std::string_view const &request_id) override;
  uint16_t operator()(
      Trace<ModifyOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;
  uint16_t operator()(
      Trace<CancelOrder> const &,
      server::oms::Order const &,
      server::oms::RefData const &,
      std::string_view const &request_id,
      std::string_view const &previous_request_id) override;

  uint16_t operator()(Trace<CancelAllOrders> const &, std::string_view const &request_id) override;

  uint16_t operator()(Trace<MassQuote> const &) override;

  uint16_t operator()(Trace<CancelQuotes> const &) override;

  void operator()(metrics::Writer &) const override;

  // Rest::Handler

  void operator()(Trace<SymbolsUpdate> const &) override;

  // OrderEntry::Handler

  void operator()(Trace<ListenKeyUpdate> const &) override;

  // DropCopy::Handler

  void operator()(Trace<DropCopy::Remove> const &) override;

  // helpers

  void check_drop_copy_zombies(TraceInfo const &);

  void ensure_symbol_slices(TraceInfo const &, size_t size);

  template <typename... Args>
  void dispatch(Args &&...);

  template <typename... Args>
  static void dispatch_helper(auto &self, Args &&...);

  template <typename Res, typename T, typename... Args>
  Res dispatch_order_action(Trace<T> const &, Args &&...);

  struct OrderEntryRR final {
    OrderEntryRR(std::vector<std::unique_ptr<OrderEntry>> &&);

    server::OrderActionStream &get_next();

    std::vector<std::unique_ptr<OrderEntry>> order_entry;

   private:
    size_t index_ = {};
  };

 private:
  server::Dispatcher &dispatcher_;
  //
  std::string const master_account_;
  //
  io::Context &context_;
  Shared shared_;
  uint16_t stream_id_ = {};
  //
  utils::unordered_map<std::string, std::unique_ptr<Account>> const accounts_;
  utils::unordered_map<std::string, std::unique_ptr<Request>> const requests_;
  //
  std::unique_ptr<Rest> rest_;
  utils::unordered_map<std::string, OrderEntryRR> order_entry_;
  utils::unordered_map<std::string, std::unique_ptr<DropCopy>> drop_copy_;
  std::vector<std::unique_ptr<server::MarketDataStream>> market_data_;
  //
  utils::unordered_set<std::string> drop_copy_zombies_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
