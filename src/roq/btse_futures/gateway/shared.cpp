/* Copyright (c) 2017-2026, Hans Erik Thrane */

#include "roq/btse_futures/gateway/shared.hpp"

namespace roq {
namespace btse_futures {
namespace gateway {

Shared::Shared(server::Dispatcher &dispatcher, Settings const &settings)
    : dispatcher{dispatcher}, settings{settings}, api{API::create(settings)}, rate_limiter{settings.misc.request_limit, settings.misc.request_limit_interval},
      symbols{settings.ws.max_subscriptions_per_stream} {
}

}  // namespace gateway
}  // namespace btse_futures
}  // namespace roq
