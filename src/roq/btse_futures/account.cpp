/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/account.hpp"

#include "roq/clock.hpp"

namespace roq {
namespace btse_futures {

// === IMPLEMENTATION ===

Account::Account(Config const &config, std::string_view const &name)
    : name(name), crypto_(config.get_api_key(name), config.get_secret(name), config.get_passphrase(name)) {
}

std::string Account::create_ws_login(std::string_view const &path) {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  return crypto_.create_ws_login(path, now_utc);
}

std::string Account::create_headers(std::string_view const &path, std::string_view const &body) {
  auto now_utc = clock::get_realtime<std::chrono::milliseconds>();
  return crypto_.create_headers(path, now_utc, body);
}

}  // namespace btse_futures
}  // namespace roq
