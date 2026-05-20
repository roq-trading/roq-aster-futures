/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/gateway/instrument.hpp"

namespace roq {
namespace aster_futures {
namespace gateway {

// === HELPERS ===

namespace {
auto create_sequencer(auto &settings) {
  auto options = market::mbp::Sequencer::Options{
      .timeout = settings.mbp.sequencer_timeout,
      .max_updates = {},
  };
  return market::mbp::Sequencer{options};
}
}  // namespace

// === IMPLEMENTATION ===

Instrument::Instrument(Settings const &settings) : sequencer_{create_sequencer(settings)} {
}

}  // namespace gateway
}  // namespace aster_futures
}  // namespace roq
