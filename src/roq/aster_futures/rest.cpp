/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/rest.hpp"

#include <algorithm>
#include <utility>

#include "roq/mask.hpp"

#include "roq/utils/update.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/aster_futures/json/map.hpp"
#include "roq/aster_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {

// === CONSTANTS ===

namespace {
auto const NAME = "rest"sv;

auto const SUPPORTS = Mask{
    SupportType::REFERENCE_DATA,
    SupportType::MARKET_STATUS,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
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
  return web::rest::Client::create(handler, context, config);
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

Rest::Rest(Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .exchange_info = create_metrics(shared.settings, name_, "exchange_info"sv),
          .exchange_info_ack = create_metrics(shared.settings, name_, "exchange_info_ack"sv),
          .depth = create_metrics(shared.settings, name_, "depth"sv),
          .depth_ack = create_metrics(shared.settings, name_, "depth_ack"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared}, download_{shared.settings.rest.request_timeout, [this](auto state) { return download(state); }} {
}

void Rest::operator()(Event<Start> const &) {
  (*connection_).start();
}

void Rest::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void Rest::operator()(Event<Timer> const &event) {
  auto &[message_info, timer] = event;
  auto now = timer.now;
  (*connection_).refresh(now);
  if (ready()) {
    check_request_queue(now);
  }
}

void Rest::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.exchange_info, metrics::Type::PROFILE)
      .write(profile_.exchange_info_ack, metrics::Type::PROFILE)
      .write(profile_.depth, metrics::Type::PROFILE)
      .write(profile_.depth_ack, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void Rest::operator()(ConnectionStatus status) {
  if (utils::update(status_, status)) {
    TraceInfo trace_info;
    auto stream_status = StreamStatus{
        .stream_id = stream_id_,
        .account = {},
        .supports = SUPPORTS,
        .transport = Transport::TCP,
        .protocol = Protocol::HTTP,
        .encoding = {Encoding::JSON},
        .priority = Priority::PRIMARY,
        .connection_status = status_,
        .interface = (*connection_).get_interface(),
        .authority = (*connection_).get_current_authority(),
        .path = (*connection_).get_current_path(),
        .proxy = (*connection_).get_proxy(),
    };
    log::info("stream_status={}"sv, stream_status);
    create_trace_and_dispatch(handler_, trace_info, stream_status);
  }
}

void Rest::operator()(Trace<web::rest::Client::Connected> const &) {
  if (download_.downloading()) {
    download_.bump();
  } else {
    (*this)(ConnectionStatus::DOWNLOADING);
    download_.begin();
  }
}

void Rest::operator()(Trace<web::rest::Client::Disconnected> const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  if (!download_.downloading()) {
    download_.reset();
  }
}

void Rest::operator()(Trace<web::rest::Client::Latency> const &event) {
  auto &[trace_info, latency] = event;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

uint32_t Rest::download(RestState state) {
  switch (state) {
    using enum RestState;
    case UNDEFINED:
      assert(false);
      break;
    case EXCHANGE_INFO:
      get_exchange_info();
      return 1;
    case DONE:
      (*this)(ConnectionStatus::READY);
      return 0;
  }
  assert(false);
  return 0;
}

// exchange_info

void Rest::get_exchange_info() {
  profile_.exchange_info([&]() {
    auto query = fmt::format("?category={}"sv, shared_.api.category);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.market_data.exchange_info,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    auto sequence = download_.sequence();
    (*connection_)("exchange_info"sv, request, [this, sequence]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_exchange_info_ack(event, sequence);
    });
  });
}

void Rest::get_exchange_info_ack(Trace<web::rest::Response> const &event, uint32_t sequence) {
  auto const state = RestState::EXCHANGE_INFO;
  profile_.exchange_info_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      download_.retry(state);
    };
    auto handle_success = [&](auto &body) {
      log::debug("{}"sv, body);
      if (download_.skip(sequence, state)) {
        log::info("Download state={} has already been processed"sv, state);
      } else {
        json::ExchangeInfoAck exchange_info_ack{body, decode_buffer_};
        Trace event{trace_info, exchange_info_ack};
        (*this)(event);
        download_.check(state);
      }
    };
    process_response(event, handle_error, handle_success);
  });
}

void Rest::operator()(Trace<json::ExchangeInfoAck> const &event) {
  auto &[trace_info, exchange_info_ack] = event;
  log::info<4>("exchange_info_ack={}"sv, exchange_info_ack);
  std::vector<Symbol> symbols;
  symbols.reserve(std::size(exchange_info_ack.symbols));
  size_t counter = 0;
  for (size_t i = 0; i < std::size(exchange_info_ack.symbols); ++i) {
    auto &item = exchange_info_ack.symbols[i];
    log::info<2>("item={}"sv, item);
    auto discard = shared_.discard_symbol(item.symbol);
    auto tick_size = NaN;
    auto min_trade_vol = NaN;
    auto max_trade_vol = NaN;
    auto trade_vol_step_size = NaN;
    for (auto &filter : item.filters) {
      switch (filter.filter_type) {
        using enum json::FilterType::type_t;
        case UNDEFINED_INTERNAL:
          break;
        case UNKNOWN_INTERNAL:
          break;
        case PRICE_FILTER:
          tick_size = filter.tick_size;
          break;
        case LOT_SIZE:
          min_trade_vol = filter.min_qty;
          max_trade_vol = filter.max_qty;
          trade_vol_step_size = filter.step_size;
          break;
        case MARKET_LOT_SIZE:
          break;
        case MAX_NUM_ORDERS:
          break;
        case MAX_NUM_ALGO_ORDERS:
          break;
        case MIN_NOTIONAL:
          break;
        case PERCENT_PRICE:
          break;
      }
    }
    auto reference_data = ReferenceData{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .description = item.name,
        .security_type = map(item.contract_type),
        .cfi_code = {},
        .base_currency = item.quote_asset,
        .quote_currency = item.base_asset,
        .settlement_currency = {},
        .margin_currency = item.margin_asset,
        .commission_currency = {},
        .tick_size = tick_size,
        .tick_size_steps = {},
        .multiplier = NaN,
        .min_notional = NaN,
        .min_trade_vol = min_trade_vol,
        .max_trade_vol = max_trade_vol,
        .trade_vol_step_size = trade_vol_step_size,
        .option_type = {},
        .strike_currency = {},
        .strike_price = NaN,
        .underlying = {},
        .time_zone = {},
        .issue_date = {},
        .settlement_date = {},
        .expiry_datetime = {},
        .expiry_datetime_utc = {},
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = exchange_info_ack.server_time,
        .discard = discard,
    };
    create_trace_and_dispatch(handler_, trace_info, reference_data, true);
    if (discard) {
      log::info<1>(R"(Drop symbol="{}")"sv, item.symbol);
      continue;
    }
    auto market_status = MarketStatus{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = item.symbol,
        .trading_status = map(item.status),
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = exchange_info_ack.server_time,
    };
    create_trace_and_dispatch(handler_, trace_info, market_status, true);
    if (shared_.all_symbols.emplace(item.symbol).second) {  // only include new
      symbols.emplace_back(item.symbol);
    }
    ++counter;
  }
  if (!std::empty(symbols)) {
    auto symbols_update = SymbolsUpdate{
        .symbols = symbols,
    };
    handler_(symbols_update);
  }
  if (counter > 0) [[unlikely]] {
    log::info("Symbols {} / {}"sv, counter, std::size(exchange_info_ack.symbols));
  }
}

// depth

void Rest::get_depth(std::string_view const &symbol) {
  profile_.depth([&]() {
    auto callback = [this, symbol = std::string{symbol}]([[maybe_unused]] auto &request_id, auto &response) {
      TraceInfo trace_info;
      Trace event{trace_info, response};
      get_depth_ack(event, symbol);
    };
    auto query = fmt::format("?symbol={}&limit={}"sv, symbol, shared_.settings.mbp.max_depth);
    auto request = web::rest::Request{
        .method = web::http::Method::GET,
        .path = shared_.api.market_data.depth,
        .query = query,
        .accept = web::http::Accept::APPLICATION_JSON,
        .content_type = {},
        .headers = {},
        .body = {},
        .quality_of_service = {},
    };
    (*connection_)("depth"sv, request, callback);
  });
}

void Rest::get_depth_ack(Trace<web::rest::Response> const &event, std::string_view const &symbol) {
  profile_.depth_ack([&]() {
    auto &[trace_info, response] = event;
    auto handle_error = [&](auto origin, auto status, auto error, auto const &text) {
      log::warn(R"(origin={}, error={}, status={}, text="{}")"sv, origin, error, status, text);
      // WHAT ???
    };
    auto handle_success = [&](auto &body) {
      log::debug("{}"sv, body);
      json::DepthAck depth_ack{body, decode_buffer_};
      Trace event{trace_info, depth_ack};
      (*this)(event, symbol);
    };
    process_response(event, handle_error, handle_success);
  });
}

void Rest::operator()(Trace<json::DepthAck> const &event, std::string_view const &symbol) {
  auto &[trace_info, depth_ack] = event;
  log::info<4>(R"(depth_ack={}, symbol="{}")"sv, depth_ack, symbol);
  auto sequence = depth_ack.last_update_id;
  auto &bids = shared_.bids;
  auto &asks = shared_.asks;
  bids.clear();
  asks.clear();
  auto emplace_back = [](auto &result, auto &item) {
    auto mbp_update = MBPUpdate{
        .price = item.price,
        .quantity = item.quantity,
        .implied_quantity = NaN,
        .number_of_orders = {},
        .update_action = {},
        .price_level = {},
    };
    result.emplace_back(std::move(mbp_update));
  };
  for (auto &item : depth_ack.bids) {
    emplace_back(bids, item);
  }
  for (auto &item : depth_ack.asks) {
    emplace_back(asks, item);
  }
  auto &instrument = shared_.get_instrument(symbol);
  auto &sequencer = instrument.get_sequencer();
  try {
    auto publish_snapshot = [&](auto &bids, auto &asks, auto sequence, auto retries, auto delay) {
      log::info(
          R"(DEBUG PUBLISH SNAPSHOT symbol="{}", sequence={}, retries={}, delay={})"sv,
          symbol,
          sequence,
          retries,
          std::chrono::duration_cast<std::chrono::milliseconds>(delay));
      auto market_by_price_update = MarketByPriceUpdate{
          .stream_id = stream_id_,
          .exchange = shared_.settings.exchange,
          .symbol = symbol,
          .bids = bids,
          .asks = asks,
          .update_type = UpdateType::SNAPSHOT,
          .exchange_time_utc = {},
          .exchange_sequence = sequencer.last_sequence(),
          .sending_time_utc = {},
          .price_precision = {},
          .quantity_precision = {},
          .checksum = {},
      };
      auto apply_updates = [&](auto &market_by_price) { sequencer.apply(market_by_price, sequence, false); };
      Trace event{trace_info, market_by_price_update};
      shared_(event, true, apply_updates);
    };
    auto request_snapshot = [&](auto retries) {
      log::info(R"(DEBUG REQUEST SNAPSHOT symbol="{}", retries={})"sv, symbol, retries);
      if (shared_.settings.ws.mbp_request_max_retries && shared_.settings.ws.mbp_request_max_retries < retries) {
        log::fatal(R"(Unexpected: symbol="{}", retries={})"sv, symbol, retries);
      }
      shared_.depth_request_queue.emplace_back(symbol);
    };
    sequencer(bids, asks, sequence, false, publish_snapshot, request_snapshot);
  } catch (BadState &) {
    log::warn(R"(RESUBSCRIBE symbol="{}")"sv, symbol);
    // XXX FIXME publish stale
    sequencer.clear();
    shared_.depth_request_queue.emplace_back(symbol);
  }
}

// helpers

void Rest::check_request_queue(std::chrono::nanoseconds now) {
  auto can_request = [&](auto now) { return shared_.rate_limiter.can_request(now); };
  auto request = [&](auto &symbol) { get_depth(symbol); };
  shared_.depth_request_queue.dispatch(can_request, request, now);
}

void Rest::process_response(web::rest::Response const &response, auto error_handler, auto success_handler) {
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
          case I_AM_A_TEAPOT:        // 418
          case TOO_MANY_REQUESTS: {  // 429
            auto message = fmt::format("{}"sv, status);
            error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::REQUEST_RATE_LIMIT_REACHED, message);
            break;
          }
          case CONFLICT:  // 409
            assert(false);
            [[fallthrough]];
          default: {
            // json::Message error{body};
            // XXX HANS error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, json::guess_error(error.code), error.msg);
          }
        }
        break;
      case SERVER_ERROR: {
        auto message = fmt::format("{}"sv, status);
        error_handler(Origin::EXCHANGE, RequestStatus::REJECTED, Error::UNKNOWN, message);
        break;
      }
    }
  } catch (NetworkError &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::GATEWAY, e.request_status(), e.error(), e.what());
  } catch (std::exception &e) {
    log::warn(R"(Exception type={}, what="{}")"sv, typeid(e).name(), e.what());
    error_handler(Origin::EXCHANGE, RequestStatus::ERROR, Error::UNKNOWN, e.what());
  }
}

}  // namespace aster_futures
}  // namespace roq
