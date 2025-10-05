/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include "roq/aster_futures/settings.hpp"

#include "roq/logging.hpp"

using namespace std::literals;

namespace roq {
namespace aster_futures {

Settings::Settings(args::Parser const &args)
    : server::flags::Settings{args, ROQ_PACKAGE_NAME, ROQ_BUILD_NUMBER}, flags::Flags{flags::Flags::create()}, misc{flags::Misc::create()},
      rest{flags::REST::create()}, ws{flags::WS::create()}, mbp{flags::MBP::create()} {
  log::info("settings={}"sv, *this);
}

}  // namespace aster_futures
}  // namespace roq
