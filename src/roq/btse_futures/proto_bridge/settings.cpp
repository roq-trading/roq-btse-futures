/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/proto_bridge/settings.hpp"

#include "roq/server/proto_bridge/flags/settings.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace proto_bridge {

// === HELPERS ===

namespace {
auto create_proto_bridge(auto &parser) {
  return server::proto_bridge::flags::Settings{parser};
}
}  // namespace

// === IMPLEMENTATION ===

Settings::Settings(args::Parser const &parser) : flags::Settings{parser}, proto_bridge{create_proto_bridge(parser)} {
}

}  // namespace proto_bridge
}  // namespace btse_futures
}  // namespace roq
