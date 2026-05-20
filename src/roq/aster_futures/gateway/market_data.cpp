/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/market_data.hpp"

#include <algorithm>

#include "roq/logging.hpp"

#include "roq/mask.hpp"

#include "roq/utils/safe_cast.hpp"
#include "roq/utils/update.hpp"

#include "roq/utils/exceptions/unhandled.hpp"

#include "roq/utils/charconv/to_string.hpp"

#include "roq/utils/metrics/factory.hpp"

#include "roq/aster_futures/json/map.hpp"
#include "roq/aster_futures/json/utils.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {
namespace gateway {

// === CONSTANTS ===

namespace {
auto const NAME = "md"sv;

auto const SUPPORTS = Mask{
    SupportType::MARKET_BY_PRICE,
    SupportType::TRADE_SUMMARY,
    SupportType::STATISTICS,
};

size_t const MAX_DECODE_BUFFER_DEPTH = 2;
}  // namespace

// === HELPERS ===

namespace {
auto create_name(auto stream_id) {
  return fmt::format("{}:{}"sv, stream_id, NAME);
}

auto create_connection(auto &handler, auto &settings, auto &context) {
  auto uri = settings.ws.uri;
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
  return web::socket::Client::create(handler, context, config, []() { return std::string(); });
}

struct create_metrics final : public utils::metrics::Factory {
  create_metrics(auto &settings, auto &group, auto const &function) : utils::metrics::Factory{settings.app.name, group, function} {}
};
}  // namespace

// === IMPLEMENTATION ===

MarketData::MarketData(Handler &handler, io::Context &context, uint16_t stream_id, Shared &shared, size_t index)
    : handler_{handler}, stream_id_{stream_id}, name_{create_name(stream_id_)}, index_{index}, connection_{create_connection(*this, shared.settings, context)},
      decode_buffer_{shared.settings.misc.decode_buffer_size, MAX_DECODE_BUFFER_DEPTH},
      counter_{
          .disconnect = create_metrics(shared.settings, name_, "disconnect"sv),
      },
      profile_{
          .parse = create_metrics(shared.settings, name_, "parse"sv),
          .pong = create_metrics(shared.settings, name_, "pong"sv),
          .ack = create_metrics(shared.settings, name_, "ack"sv),
          .agg_trade = create_metrics(shared.settings, name_, "agg_trade"sv),
          .mark_price_update = create_metrics(shared.settings, name_, "mark_price_update"sv),
          .mini_ticker = create_metrics(shared.settings, name_, "mini_ticker"sv),
          .ticker = create_metrics(shared.settings, name_, "ticker"sv),
          .book_ticker = create_metrics(shared.settings, name_, "book_ticker"sv),
          .depth_update = create_metrics(shared.settings, name_, "depth_update"sv),
      },
      latency_{
          .ping = create_metrics(shared.settings, name_, "ping"sv),
      },
      shared_{shared}, rate_limiter_{shared_.settings.ws.request_limit, shared_.settings.ws.request_limit_interval},
      request_queue_{shared_.settings.ws.request_delay} {
}

void MarketData::operator()(Event<Start> const &) {
  (*connection_).start();
}

void MarketData::operator()(Event<Stop> const &) {
  (*connection_).stop();
}

void MarketData::operator()(Event<Timer> const &event) {
  auto now = event.value.now;
  (*connection_).refresh(now);
  if (ready()) {
    check_request_queue(now);
  }
}

void MarketData::operator()(metrics::Writer &writer) const {
  writer
      // counter
      .write(counter_.disconnect, metrics::Type::COUNTER)
      // profile
      .write(profile_.parse, metrics::Type::PROFILE)
      .write(profile_.pong, metrics::Type::PROFILE)
      .write(profile_.ack, metrics::Type::PROFILE)
      .write(profile_.agg_trade, metrics::Type::PROFILE)
      .write(profile_.mark_price_update, metrics::Type::PROFILE)
      .write(profile_.mini_ticker, metrics::Type::PROFILE)
      .write(profile_.ticker, metrics::Type::PROFILE)
      .write(profile_.book_ticker, metrics::Type::PROFILE)
      .write(profile_.depth_update, metrics::Type::PROFILE)
      // latency
      .write(latency_.ping, metrics::Type::LATENCY);
}

void MarketData::subscribe(size_t start_from) {
  if (ready()) {
    subscribe(shared_.symbols.get_slice(index_, start_from));
  }
}

void MarketData::operator()(web::socket::Client::Connected const &) {
}

void MarketData::operator()(web::socket::Client::Disconnected const &) {
  ++counter_.disconnect;
  (*this)(ConnectionStatus::DISCONNECTED);
  request_queue_.clear();
}

void MarketData::operator()(web::socket::Client::Ready const &) {
  (*this)(ConnectionStatus::READY);
  subscribe();
}

void MarketData::operator()(web::socket::Client::Close const &) {
}

void MarketData::operator()(web::socket::Client::Latency const &latency) {
  TraceInfo trace_info;
  auto external_latency = ExternalLatency{
      .stream_id = stream_id_,
      .account = {},
      .latency = latency.sample,
  };
  create_trace_and_dispatch(handler_, trace_info, external_latency);
  latency_.ping.update(latency.sample);
}

void MarketData::operator()(web::socket::Client::Text const &text) {
  parse(text.payload);
}

void MarketData::operator()(web::socket::Client::Binary const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(ConnectionStatus connection_status, std::string_view const &reason) {
  connection_status_ = connection_status;
  TraceInfo trace_info;
  auto stream_status = StreamStatus{
      .stream_id = stream_id_,
      .account = {},
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
  create_trace_and_dispatch(handler_, trace_info, stream_status);
}

void MarketData::subscribe(std::span<Symbol const> const &symbols) {
  if (std::empty(symbols)) {
    return;
  }
  std::array<std::string_view, 6> const streams{{
      "aggTrade"sv,
      "markPrice@1s"sv,
      "miniTicker"sv,
      "ticker"sv,
      "bookTicker"sv,
      "depth"sv,
  }};
  subscribe(symbols, streams);
}

void MarketData::subscribe(std::span<Symbol const> const &symbols, std::span<std::string_view const> const &streams) {
  assert(!std::empty(symbols));
  for (auto &item : symbols) {
    std::string symbol{item};
    std::ranges::transform(symbol, std::begin(symbol), [](auto value) { return std::tolower(value); });
    auto separator = fmt::format(R"(","{}@)"sv, symbol);
    auto message = fmt::format(
        R"({{)"
        R"("id":{},)"
        R"("method":"SUBSCRIBE",)"
        R"("params":[)"
        R"("{}@{}")"
        R"(])"
        R"(}})"sv,
        ++request_id_,
        symbol,
        fmt::join(streams, separator));
    request_queue_.emplace_back(std::move(message));
  }
}

void MarketData::check_request_queue(std::chrono::nanoseconds now) {
  auto can_request = [&](auto now) { return rate_limiter_.can_request(now); };
  auto request = [&](auto &message) {
    log::debug("{}"sv, message);
    (*connection_).send_text(message);
  };
  request_queue_.dispatch(can_request, request, now);
}

void MarketData::parse(std::string_view const &message) {
  profile_.parse([&]() {
    auto log_message = [&]() { log::warn(R"(*** PLEASE REPORT *** message="{}")"sv, message); };
    try {
      TraceInfo trace_info;
      if (!json::Parser::dispatch(*this, message, decode_buffer_, trace_info, shared_.settings.experimental.allow_unknown_event_types)) {
        log_message();
      }
    } catch (...) {
      log_message();
      utils::exceptions::Unhandled::terminate();
    }
  });
}

void MarketData::operator()(Trace<json::Pong> const &event) {
  profile_.pong([&]() {
    auto &[trace_info, pong] = event;
    auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(trace_info.source_receive_time) - std::chrono::milliseconds{pong.id};
    log::warn("DEBUG pong={}, latency={}"sv, pong, latency);
    // XXX HANS ExternalLatency
  });
}

void MarketData::operator()(Trace<json::Ack> const &event) {
  profile_.ack([&]() {
    auto &[trace_info, ack] = event;
    log::warn("DEBUG ack={}"sv, ack);
  });
}

void MarketData::operator()(Trace<json::AggTrade> const &event) {
  profile_.agg_trade([&]() {
    auto &[trace_info, agg_trade] = event;
    log::info<4>("agg_trade={}"sv, agg_trade);
    auto trade = Trade{
        .side = agg_trade.is_buyer_maker ? Side::SELL : Side::BUY,  // note! we need the side of the taker
        .price = agg_trade.price,
        .quantity = agg_trade.quantity,
        .trade_id = {},
        .taker_order_id = {},
        .maker_order_id = {},
    };
    utils::charconv::to_string(std::back_inserter(trade.trade_id), agg_trade.trade_id);
    auto trade_summary = TradeSummary{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = agg_trade.symbol,
        .trades = {&trade, 1},
        .exchange_time_utc = utils::safe_cast(agg_trade.trade_time),
        .exchange_sequence = {},
        .sending_time_utc = utils::safe_cast(agg_trade.event_time),
    };
    create_trace_and_dispatch(handler_, trace_info, trade_summary, true);
  });
}

void MarketData::operator()(Trace<json::MarkPriceUpdate> const &event) {
  profile_.mark_price_update([&]() {
    auto &[trace_info, mark_price_update] = event;
    log::info<4>("mark_price_update={}"sv, mark_price_update);
    std::array<Statistics, 4> statistics{{
        {
            .type = StatisticsType::SETTLEMENT_PRICE,
            .value = mark_price_update.mark_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::INDEX_VALUE,
            .value = mark_price_update.index_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::PRE_SETTLEMENT_PRICE,
            .value = mark_price_update.estimated_settle_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::FUNDING_RATE,
            .value = mark_price_update.funding_rate,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
    }};
    auto statistics_update = StatisticsUpdate{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = mark_price_update.symbol,
        .statistics = statistics,
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = mark_price_update.trade_time,
        .exchange_sequence = {},
        .sending_time_utc = mark_price_update.event_time,
    };
    create_trace_and_dispatch(handler_, trace_info, statistics_update, true);
  });
}

void MarketData::operator()(Trace<json::MiniTicker> const &event) {
  profile_.mini_ticker([&]() {
    auto &[trace_info, mini_ticker] = event;
    log::info<4>("mini_ticker={}"sv, mini_ticker);
    std::array<Statistics, 5> statistics{{
        {
            .type = StatisticsType::CLOSE_PRICE,
            .value = mini_ticker.close_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::OPEN_PRICE,
            .value = mini_ticker.open_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::HIGHEST_TRADED_PRICE,
            .value = mini_ticker.high_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::LOWEST_TRADED_PRICE,
            .value = mini_ticker.low_price,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        {
            .type = StatisticsType::TRADE_VOLUME,
            .value = mini_ticker.total_volume_base,
            .begin_time_utc = {},
            .end_time_utc = {},
        },
        // total_volume_quote ???
    }};
    auto statistics_update = StatisticsUpdate{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = mini_ticker.symbol,
        .statistics = statistics,
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = {},
        .exchange_sequence = {},
        .sending_time_utc = mini_ticker.event_time,
    };
    create_trace_and_dispatch(handler_, trace_info, statistics_update, true);
  });
}

void MarketData::operator()(Trace<json::Ticker> const &event) {
  profile_.ticker([&]() {
    auto &[trace_info, ticker] = event;
    log::info<4>("ticker={}"sv, ticker);
    // note! Ticker is enough
  });
}

void MarketData::operator()(Trace<json::BookTicker> const &event) {
  profile_.book_ticker([&]() {
    auto &[trace_info, book_ticker] = event;
    log::info<4>("book_ticker={}"sv, book_ticker);
    auto top_of_book = TopOfBook{
        .stream_id = stream_id_,
        .exchange = shared_.settings.exchange,
        .symbol = book_ticker.symbol,
        .layer{
            .bid_price = book_ticker.best_bid_price,
            .bid_quantity = book_ticker.best_bid_quantity,
            .ask_price = book_ticker.best_ask_price,
            .ask_quantity = book_ticker.best_ask_quantity,
        },
        .update_type = UpdateType::INCREMENTAL,
        .exchange_time_utc = book_ticker.transaction_time,
        .exchange_sequence = utils::safe_cast(book_ticker.order_book_update_id),
        .sending_time_utc = book_ticker.event_time,
    };
    create_trace_and_dispatch(handler_, trace_info, top_of_book, true);
  });
}

void MarketData::operator()(Trace<json::DepthUpdate> const &event) {
  profile_.depth_update([&]() {
    auto &[trace_info, depth_update] = event;
    log::info<4>("depth_update={}"sv, depth_update);
    auto helper = [](auto &result, auto &item) {
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
    auto &bids = shared_.bids;
    auto &asks = shared_.asks;
    bids.clear();
    asks.clear();
    for (auto &item : depth_update.bids) {
      helper(bids, item);
    }
    for (auto &item : depth_update.asks) {
      helper(asks, item);
    }
    auto &instrument = shared_.get_instrument(depth_update.symbol);
    auto &sequencer = instrument.get_sequencer();
    auto first_sequence = depth_update.first_update_id;
    auto last_sequence = depth_update.final_update_id;
    auto previous_sequence = depth_update.previous_final_update_id;
    try {
      auto create_update = [&](auto &bids, auto &asks, auto update_type, auto exchange_sequence) -> MarketByPriceUpdate {
        return {
            .stream_id = stream_id_,
            .exchange = shared_.settings.exchange,
            .symbol = depth_update.symbol,
            .bids = bids,
            .asks = asks,
            .update_type = update_type,
            .exchange_time_utc = {},
            .exchange_sequence = exchange_sequence,
            .sending_time_utc = depth_update.event_time,
            .price_precision = {},
            .quantity_precision = {},
            .max_depth = {},
            .checksum = {},
        };
      };
      auto publish_update = [&](auto &bids, auto &asks) {
        auto market_by_price_update = create_update(bids, asks, UpdateType::INCREMENTAL, last_sequence);
        create_trace_and_dispatch(handler_, trace_info, market_by_price_update, true);
      };
      auto publish_snapshot = [&](auto &bids, auto &asks, auto sequence, auto retries, auto delay) {
        log::info(
            R"(DEBUG PUBLISH SNAPSHOT symbol="{}", sequence={}, retries={}, delay={})"sv,
            depth_update.symbol,
            sequence,
            retries,
            std::chrono::duration_cast<std::chrono::milliseconds>(delay));
        auto market_by_price_update = create_update(bids, asks, UpdateType::SNAPSHOT, sequencer.last_sequence());
        auto apply_updates = [&](auto &market_by_price) { sequencer.apply(market_by_price, sequence, false); };
        Trace event{trace_info, market_by_price_update};
        shared_(event, true, apply_updates);
      };
      auto request_snapshot = [&](auto retries) {
        log::info(R"(DEBUG REQUEST SNAPSHOT symbol="{}", retries={})"sv, depth_update.symbol, retries);
        if (shared_.settings.ws.mbp_request_max_retries && shared_.settings.ws.mbp_request_max_retries < retries) {
          log::fatal(R"(Unexpected: symbol="{}", retries={})"sv, depth_update.symbol, retries);
        }
        shared_.depth_request_queue.emplace_back(depth_update.symbol);
      };
      sequencer(bids, asks, first_sequence, last_sequence, previous_sequence, publish_update, publish_snapshot, request_snapshot);
    } catch (BadState &) {
      log::warn(R"(RESUBSCRIBE symbol="{}")"sv, depth_update.symbol);
      // XXX HANS publish stale
      sequencer.clear();
      shared_.depth_request_queue.emplace_back(depth_update.symbol);
    }
  });
}

void MarketData::operator()(Trace<json::Login> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Account> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Position> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Order> const &) {
  log::fatal("Unexpected"sv);
}

void MarketData::operator()(Trace<json::Fill> const &) {
  log::fatal("Unexpected"sv);
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
