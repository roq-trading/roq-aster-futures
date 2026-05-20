/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include "roq/market/mbp/sequencer.hpp"

#include "roq/aster_futures/gateway/settings.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

struct Instrument final {
  explicit Instrument(Settings const &);

  Instrument(Instrument &&) = default;
  Instrument(Instrument const &) = delete;

  auto &get_sequencer() { return sequencer_; }

 private:
  market::mbp::Sequencer sequencer_;
};

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
