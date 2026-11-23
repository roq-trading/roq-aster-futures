/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/controller.hpp"

#include "roq/logging.hpp"

#include "roq/server/oms/exceptions.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === HELPERS ===

namespace {
template <typename R>
R create_accounts(auto &settings, auto &config) {
  using value_type = std::remove_cvref_t<R>;
  value_type result;
  for (auto &[_, account] : config.accounts) {
    auto obj = std::make_unique<Account>(settings, config, account.name);
    result.try_emplace(account.name, std::move(obj));
  }
  return result;
}

Account &get_account(auto &accounts, auto &account) {
  auto iter = accounts.find(account);
  if (iter == std::end(accounts)) {
    log::fatal("Unexpected"sv);
  }
  return *(*iter).second;
}

template <typename R>
R create_requests(auto &config, auto &shared) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  for (auto &[_, account] : config.accounts) {
    auto obj = std::make_unique<Request>(shared);
    result.try_emplace(account.name, std::move(obj));
  }
  return result;
}

Request &get_request(auto &requests, auto &account) {
  auto iter = requests.find(account);
  if (iter == std::end(requests)) {
    log::fatal("Unexpected"sv);
  }
  return *(*iter).second;
}

auto create_rest(auto &gateway, auto &context, auto &stream_id, auto &shared) {
  return std::make_unique<Rest>(gateway, context, ++stream_id, shared);
}

template <typename R>
R create_order_entry(auto &gateway, auto &context, auto &stream_id, auto &accounts, auto &shared, auto &requests) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  if (!shared.settings.misc.number_of_order_entry_connections) {
    log::fatal("Unexpected: --number_of_order_entry_connections={}"sv, shared.settings.misc.number_of_order_entry_connections);
  }
  for (auto &[_, item] : accounts) {
    auto &account = *item;
    auto &request = get_request(requests, account.name);
    std::vector<std::unique_ptr<OrderEntry>> order_entry;
    for (size_t i = 0; i < shared.settings.misc.number_of_order_entry_connections; ++i) {
      auto obj = std::make_unique<OrderEntry>(gateway, context, ++stream_id, account, shared, request, i == 0);
      order_entry.emplace_back(std::move(obj));
    }
    result.try_emplace(account.name, std::move(order_entry));
  }
  return result;
}

template <typename R>
R create_drop_copy(auto &accounts) {
  using result_type = std::remove_cvref_t<R>;
  result_type result;
  for (auto &[_, item] : accounts) {
    auto &account = *item;
    result.try_emplace(account.name, nullptr);
  }
  return result;
}
}  // namespace

// === IMPLEMENTATION ===

std::unique_ptr<server::Handler> Controller::create(server::Dispatcher &dispatcher, Settings const &settings, Config const &config, io::Context &context) {
  return std::make_unique<Controller>(dispatcher, settings, config, context);
}

uint8_t Controller::parse_api(Settings const &) {
  return {};
}

Controller::Controller(server::Dispatcher &dispatcher, Settings const &settings, Config const &config, io::Context &context)
    : dispatcher_{dispatcher}, master_account_{config.get_master_account()}, context_{context}, shared_{dispatcher, settings},
      accounts_{create_accounts<decltype(accounts_)>(settings, config)}, requests_{create_requests<decltype(requests_)>(config, shared_)},
      rest_{create_rest(*this, context_, stream_id_, shared_)},
      order_entry_{create_order_entry<decltype(order_entry_)>(*this, context_, stream_id_, accounts_, shared_, requests_)},
      drop_copy_{create_drop_copy<decltype(drop_copy_)>(accounts_)} {
}

// server::Handler

void Controller::operator()(Trace<Start> const &event) {
  auto &[trace_info, start] = event;
  log::info("Starting..."sv);
  assert(std::empty(market_data_));
  dispatch(event);
  // create listen-keys
  for (auto &[_, item] : requests_) {
    (*item).listen_key.request(trace_info);
  }
}

void Controller::operator()(Trace<Stop> const &event) {
  log::info("Stopping..."sv);
  dispatch(event);
}

void Controller::operator()(Trace<Timer> const &event) {
  auto &[trace_info, timer] = event;
  check_drop_copy_zombies(trace_info);  // note! remove *BEFORE* dispatching timer
  dispatch(event);
}

void Controller::operator()(Trace<Connected> const &, [[maybe_unused]] uint8_t user_id) {
}

void Controller::operator()(Trace<Disconnected> const &, [[maybe_unused]] uint8_t user_id) {
}

void Controller::operator()(Trace<Subscribe> const &event) {
  auto &[trace_info, subscribe] = event;
  std::vector<Symbol> symbols;
  for (auto &item : subscribe.symbols) {
    if (shared_.all_symbols.emplace(item).second) {
      symbols.emplace_back(item);
    } else {
      log::warn(R"(*** DUPLICATE SUBSCRIPTION *** (symbol="{}")"sv, item);
    }
  }
  auto symbols_update = SymbolsUpdate{
      .symbols = symbols,
  };
  create_trace_and_dispatch_2(trace_info, symbols_update);
}

void Controller::operator()(Trace<Control> const &event) {
  auto &[trace_info, control] = event;
  switch (control.action) {
    using enum Action;
    case UNDEFINED:
      assert(false);
      break;
    case ENABLE:
      dispatcher_(State::ENABLED);
      break;
    case DISABLE:
      dispatcher_(State::DISABLED);
      break;
  }
}

void Controller::operator()(Trace<server::oms::OrderActionBatchBegin> const &event) {
  dispatch_order_action(event);
}

void Controller::operator()(Trace<server::oms::OrderActionBatchCommit> const &event) {
  dispatch_order_action(event);
}

void Controller::operator()(Trace<server::oms::OrderActionBatchRollback> const &event) {
  dispatch_order_action(event);
}

uint16_t Controller::operator()(
    Trace<CreateOrder> const &event, server::oms::Order const &order, server::oms::RefData const &ref_data, std::string_view const &request_id) {
  return dispatch_order_action(event, order, ref_data, request_id);
}

uint16_t Controller::operator()(
    Trace<ModifyOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  return dispatch_order_action(event, order, ref_data, request_id, previous_request_id);
}

uint16_t Controller::operator()(
    Trace<CancelOrder> const &event,
    server::oms::Order const &order,
    server::oms::RefData const &ref_data,
    std::string_view const &request_id,
    std::string_view const &previous_request_id) {
  return dispatch_order_action(event, order, ref_data, request_id, previous_request_id);
}

uint16_t Controller::operator()(Trace<CancelAllOrders> const &event, std::string_view const &request_id) {
  return dispatch_order_action(event, request_id);
}

uint16_t Controller::operator()(Trace<MassQuote> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

uint16_t Controller::operator()(Trace<CancelQuotes> const &) {
  throw server::oms::NotSupported{"not supported"sv};
}

void Controller::operator()(metrics::Writer &writer) const {
  dispatch_helper(*this, writer);
}

// Rest::Handler

void Controller::operator()(Trace<SymbolsUpdate> const &event) {
  auto &[trace_info, symbols_update] = event;
  auto [size, start_from] = shared_.symbols(symbols_update.symbols);
  ensure_symbol_slices(trace_info, size);
  for (auto &iter : market_data_) {
    (*iter).subscribe(start_from);
  }
}

// OrderEntry::Handler

void Controller::operator()(Trace<ListenKeyUpdate> const &event) {
  auto &[trace_info, listen_key_update] = event;
  auto &account = listen_key_update.account;
  assert(!std::empty(account));
  auto iter = drop_copy_.find(account);
  if (iter == std::end(drop_copy_)) {
    log::fatal(R"(Unexpected: account="{}")"sv, account);
  } else if (!static_cast<bool>((*iter).second)) {
    log::info(R"(Create DropCopy (user-stream) for account="{}" using listen_key="{}")"sv, account, listen_key_update.listen_key);
    auto &account_2 = get_account(accounts_, account);
    auto &request = get_request(requests_, account);
    auto obj = std::make_unique<DropCopy>(*this, context_, ++stream_id_, account_2, shared_, request, listen_key_update.listen_key);
    Start start;
    create_trace_and_dispatch(dynamic_cast<server::Stream &>(*obj), trace_info, start);
    (*iter).second = std::move(obj);
  } else {
    (*(*iter).second)(event);
  }
}

// DropCopy::Handler

void Controller::operator()(Trace<DropCopy::Remove> const &event) {
  auto &[trace_info, remove] = event;
  log::info(R"(Requesting DropCopy (user-stream) to be removed for account="{}"...)"sv, remove.account);
  drop_copy_zombies_.insert(remove.account);
}

// helpers

void Controller::check_drop_copy_zombies(TraceInfo const &trace_info) {
  if (std::empty(drop_copy_zombies_)) {
    return;
  }
  for (auto &account : drop_copy_zombies_) {
    log::info(R"(Removing DropCopy (user-stream) for account="{}"...)"sv, account);
    auto iter = drop_copy_.find(account);
    if (iter == std::end(drop_copy_)) [[unlikely]] {
      log::fatal("Unexpected"sv);
    }
    (*iter).second.reset();
    log::info(R"(DropCopy (user-stream) was removed for account="{}")"sv, account);
    //
    log::warn("DEBUG requesting new listen-key..."sv);
    get_request(requests_, account).listen_key.request(trace_info);
  }
  drop_copy_zombies_.clear();
}

void Controller::ensure_symbol_slices(TraceInfo const &trace_info, size_t size) {
  while (std::size(market_data_) < size) {
    log::debug("Create market-data (user-stream)"sv);
    auto obj = std::make_unique<MarketData>(*this, context_, ++stream_id_, shared_, std::size(market_data_));
    Start start;
    create_trace_and_dispatch(dynamic_cast<server::Stream &>(*obj), trace_info, start);
    market_data_.emplace_back(std::move(obj));
  }
}

template <typename... Args>
void Controller::dispatch(Args &&...args) {
  dispatch_helper(*this, std::forward<Args>(args)...);
}

template <typename... Args>
void Controller::dispatch_helper(auto &self, Args &&...args) {
  auto helper = [&](auto &target) {
    if constexpr (std::is_const_v<decltype(self)>) {
      dynamic_cast<server::Stream const &>(target)(std::forward<Args>(args)...);
    } else {
      dynamic_cast<server::Stream &>(target)(std::forward<Args>(args)...);
    }
  };
  helper(*self.rest_);
  for (auto &[_, order_entry] : self.order_entry_) {
    for (auto &item : order_entry.order_entry) {
      helper(*item);
    }
  }
  for (auto &[_, drop_copy] : self.drop_copy_) {
    if (static_cast<bool>(drop_copy)) {
      helper(*drop_copy);
    }
  }
  for (auto &iter : self.market_data_) {
    helper(*iter);
  }
}

template <typename T, typename... Args>
uint16_t Controller::dispatch_order_action(Trace<T> const &event, Args &&...args) {
  auto &[trace_info, value] = event;
  auto iter = order_entry_.find(value.account);
  if (iter == std::end(order_entry_)) [[unlikely]] {
    throw RuntimeError(R"(Unknown account="{}")"sv, value.account);
  }
  auto &stream = (*iter).second.get_next();
  stream(event, args...);
  return stream.stream_id();
}

// OrderEntryRR

Controller::OrderEntryRR::OrderEntryRR(std::vector<std::unique_ptr<OrderEntry>> &&order_entry) : order_entry{std::move(order_entry)} {
}

server::OrderActionStream &Controller::OrderEntryRR::get_next() {
  auto length = std::size(order_entry);
  for (size_t offset = 0; offset < length; ++offset) {
    auto index = (index_ + offset) % length;
    auto &obj = dynamic_cast<server::OrderActionStream &>(*(order_entry[index]));
    if (!obj.ready()) {
      continue;
    }
    index_ = (index + 1) % length;
    return obj;
  }
  throw server::oms::NotReady{"get_next"sv};
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
