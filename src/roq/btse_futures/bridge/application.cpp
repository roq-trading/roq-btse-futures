/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/bridge/application.hpp"

#include "roq/logging.hpp"

#include "roq/server/bridge/controller.hpp"

#include "roq/btse_futures/gateway/controller.hpp"

#include "roq/btse_futures/bridge/config.hpp"
#include "roq/btse_futures/bridge/settings.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace bridge {

// === IMPLEMENTATION ===

int Application::main(args::Parser const &args) {
  Settings settings{args};
  Config config{settings};
  log::warn("config={}"sv, config);
  auto context = server::create_io_context(settings);
  server::bridge::Controller<gateway::Controller>{settings, config, *context}.dispatch();
  return EXIT_SUCCESS;
}

}  // namespace bridge
}  // namespace btse_futures
}  // namespace roq
