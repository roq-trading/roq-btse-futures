/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/tools/crypto.hpp"

#include <fmt/format.h>

#include <cassert>

#include "roq/utils/codec/base64.hpp"
#include "roq/utils/codec/hex.hpp"

using namespace std::literals;

namespace roq {
namespace btse_futures {
namespace tools {

// === IMPLEMENTATION ===

Crypto::Crypto(std::string_view const &key, std::string_view const &secret, std::string_view const &passphrase)
    : key_{key}, mac_{secret}, passphrase_{passphrase} {
}

std::string Crypto::create_ws_login(std::string_view const &path, std::chrono::milliseconds now_utc) {
  assert(!std::empty(path));
  auto tmp = fmt::format("{}{}"sv, path, now_utc.count());
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Hex::encode(signature, digest);
  auto result = fmt::format(
      R"({{)"
      R"("op":"authKeyExpires",)"
      R"("args":["{}","{}","{}"])"
      R"(}})"sv,
      key_,
      now_utc.count(),
      signature);
  return result;
}

std::string Crypto::create_headers(std::string_view const &path, std::chrono::milliseconds now_utc, std::string_view const &body) {
  assert(!std::empty(path));
  auto tmp = fmt::format("{}{}{}"sv, path, now_utc.count(), body);
  mac_.clear();
  mac_.update(tmp);
  auto digest = mac_.final(digest_);
  std::string signature;
  utils::codec::Hex::encode(signature, digest);
  auto result = fmt::format(
      "request-api: {}\r\n"
      "request-nonce: {}\r\n"
      "request-sign: {}\r\n"sv,
      key_,
      now_utc.count(),
      signature);
  return result;
}

}  // namespace tools
}  // namespace btse_futures
}  // namespace roq
