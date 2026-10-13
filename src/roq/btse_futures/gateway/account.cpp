/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/gateway/account.hpp"

#include "roq/clock.hpp"

#include "roq/utils/safe_cast.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

// === IMPLEMENTATION ===

Account::Account(Config const &config, std::string_view const &name)
    : name{name}, crypto_{config.get_api_key(name), config.get_secret(name), config.get_passphrase(name)} {
}

std::string Account::create_ws_login(TraceInfo const &trace_info, std::string_view const &path) {
  return crypto_.create_ws_login(path, utils::safe_cast(trace_info.origin_create_time_utc));
}

std::string Account::create_headers(TraceInfo const &trace_info, std::string_view const &path, std::string_view const &body) {
  return crypto_.create_headers(path, utils::safe_cast(trace_info.origin_create_time_utc), body);
}

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
