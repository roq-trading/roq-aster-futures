/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/aster_futures/instrument.hpp"

namespace roq {
namespace aster_futures {

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

}  // namespace aster_futures
}  // namespace roq
