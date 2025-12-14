/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <string>
#include <string_view>

#include "roq/web/http/method.hpp"

#include "roq/btse_futures/config.hpp"

#include "roq/btse_futures/tools/crypto.hpp"

namespace roq {
namespace btse_futures {

class Account final {
 public:
  Account(Config const &, std::string_view const &name);

  Account(Account const &) = delete;

  std::string create_ws_login(std::string_view const &path);

  std::string create_headers(std::string_view const &path) { return create_headers(path, {}); }

  std::string create_headers(std::string_view const &path, std::string_view const &body);

  std::string const name;

 private:
  tools::Crypto crypto_;
};

}  // namespace btse_futures
}  // namespace roq
