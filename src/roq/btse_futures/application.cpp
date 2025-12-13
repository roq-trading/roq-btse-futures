/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/application.hpp"

#include "roq/btse_futures/config.hpp"
#include "roq/btse_futures/gateway.hpp"
#include "roq/btse_futures/settings.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === CONSTANTS ===

namespace {
uint8_t const API_2 = {};
}

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  Settings settings{args};
  Config config{settings};
  log::info<1>("config={}"sv, config);
  auto context = server::create_io_context(settings);
  server::Trading<Gateway>(settings, config, *context, API_2).dispatch();
  return EXIT_SUCCESS;
}

}  // namespace btse_futures
}  // namespace roq
