/* Copyright (c) 2017-2026, Hans Erik Thrane */

#pragma once

#include <array>
#include <chrono>
#include <string>
#include <string_view>

#include "roq/utils/mac/hmac.hpp"

namespace roq {
namespace btse_futures {
namespace tools {

class Crypto final {
 public:
  Crypto(std::string_view const &key, std::string_view const &secret, std::string_view const &passphrase);

  Crypto(Crypto &&) = delete;
  Crypto(Crypto const &) = delete;

  std::string create_ws_login(std::string_view const &path, std::chrono::milliseconds now_utc);

  std::string create_headers(std::string_view const &path, std::chrono::milliseconds now_utc, std::string_view const &body);

 private:
  using MAC = utils::mac::HMAC<utils::hash::SHA384>;
  using Digest = std::array<std::byte, MAC::DIGEST_LENGTH>;

  std::string const key_;
  MAC mac_;
  Digest digest_;
  std::string const passphrase_;
};

}  // namespace tools
}  // namespace btse_futures
}  // namespace roq
