/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/application.hpp"

#include "roq/btse_futures/flags/settings.hpp"

#include "roq/btse_futures/gateway/config.hpp"
#include "roq/btse_futures/gateway/controller.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  flags::Settings settings{args};
  gateway::Config config{settings};
  auto context = server::create_io_context(settings);
  server::Trading<gateway::Controller>(settings, config, *context).dispatch();
  return EXIT_SUCCESS;
}

}  // namespace btse_futures
}  // namespace roq
