/* Copyright (c) 2017-2025, Hans Erik Thrane */

#pragma once

#include "roq/market/mbp/sequencer.hpp"

#include "roq/aster_futures/settings.hpp"

namespace roq {
namespace aster_futures {

struct Instrument final {
  explicit Instrument(Settings const &);

  Instrument(Instrument &&) = default;
  Instrument(Instrument const &) = delete;

  auto &get_sequencer() { return sequencer_; }

 private:
  market::mbp::Sequencer sequencer_;
};

}  // namespace aster_futures
}  // namespace roq
