/* Copyright (c) 2017-2025, Hans Erik Thrane */

#include <catch2/catch_all.hpp>

#include "roq/phemex/json/book.hpp"
#include "roq/phemex/json/parser.hpp"

using namespace roq;
using namespace roq::phemex;

using namespace std::literals;

// note! truncated
TEST_CASE("snapshot", "[json_book]") {
  auto message = R"({)"
                 R"("book":{)"
                 R"("asks":[)"
                 R"([1094473000,1307083],)"
                 R"([1094528000,1806],)"
                 R"([1094759000,995],)"
                 R"([1094764000,238344])"
                 R"(],)"
                 R"("bids":[)"
                 R"([1094472000,190034],)"
                 R"([1094471000,804],)"
                 R"([1094253000,40019],)"
                 R"([1094251000,1])"
                 R"(])"
                 R"(},)"
                 R"("depth":30,)"
                 R"("sequence":20256544456,)"
                 R"("symbol":"BTCUSD",)"
                 R"("timestamp":1759032884308731357,)"
                 R"("type":"snapshot")"
                 R"(})";
  core::json::BufferStack buffer_stack{8192, 3};
  struct MyHandler : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) { FAIL(); }
    void operator()(Trace<json::Ack> const &) { FAIL(); }
    void operator()(Trace<json::Book> const &event) {
      ++count;
      auto &[trace_info, book] = event;
      /*
      CHECK(book.action == json::Action::UPDATE);
      CHECK(book.arg.symbol == "ETHUSDT"sv);
      REQUIRE(std::size(book.data) == 1);
      auto &data = book.data[0];
      auto &asks = data.asks;
      REQUIRE(std::size(asks) == 12);
      CHECK(asks[0].price == Catch::Approx{4488.46});
      CHECK(asks[0].size == Catch::Approx{1.52});
      CHECK(asks[11].price == Catch::Approx{4502.33});
      CHECK(asks[11].size == Catch::Approx{91.72});
      auto &bids = data.bids;
      REQUIRE(std::size(bids) == 9);
      CHECK(bids[0].price == Catch::Approx{4487.66});
      CHECK(bids[0].size == Catch::Approx{34.16});
      CHECK(bids[8].price == Catch::Approx{4474.89});
      CHECK(bids[8].size == Catch::Approx{92.87});
      CHECK(data.checksum == 1990051938);
      CHECK(data.pseq == 1352182127478939650);
      CHECK(data.seq == 1352182127898370048);
      CHECK(data.ts == 1758111262297ms);
      CHECK(book.ts == 1758111262300ms);
      */
    }
    void operator()(Trace<json::Trades> const &) { FAIL(); }
    void operator()(Trace<json::Market24h> const &) { FAIL(); }
    void operator()(Trace<json::Kline> const &) { FAIL(); }
    void operator()(Trace<json::Login> const &) { FAIL(); }
    void operator()(Trace<json::Account> const &) { FAIL(); }
    void operator()(Trace<json::Position> const &) { FAIL(); }
    void operator()(Trace<json::Order> const &) { FAIL(); }
    void operator()(Trace<json::Fill> const &) { FAIL(); }
    size_t count = 0;
  } handler;
  TraceInfo trace_info;
  json::Parser::dispatch(handler, message, buffer_stack, trace_info, false);
  CHECK(handler.count == 1);
}

TEST_CASE("incremental", "[json_book]") {
  auto message = R"({)"
                 R"("book":{)"
                 R"("asks":[],)"
                 R"("bids":[)"
                 R"([399734000000,0],)"
                 R"([398573000000,40000])"
                 R"(])"
                 R"(},)"
                 R"("depth":30,)"
                 R"("sequence":38514847027,)"
                 R"("symbol":"sETHUSDT",)"
                 R"("timestamp":1759033283091421157,)"
                 R"("type":"incremental")"
                 R"(})";
  core::json::BufferStack buffer_stack{8192, 3};
  struct MyHandler : public json::Parser::Handler {
    void operator()(Trace<json::Pong> const &) { FAIL(); }
    void operator()(Trace<json::Ack> const &) { FAIL(); }
    void operator()(Trace<json::Book> const &event) {
      ++count;
      auto &[trace_info, book] = event;
      /*
      CHECK(book.action == json::Action::UPDATE);
      CHECK(book.arg.symbol == "ETHUSDT"sv);
      REQUIRE(std::size(book.data) == 1);
      auto &data = book.data[0];
      auto &asks = data.asks;
      REQUIRE(std::size(asks) == 12);
      CHECK(asks[0].price == Catch::Approx{4488.46});
      CHECK(asks[0].size == Catch::Approx{1.52});
      CHECK(asks[11].price == Catch::Approx{4502.33});
      CHECK(asks[11].size == Catch::Approx{91.72});
      auto &bids = data.bids;
      REQUIRE(std::size(bids) == 9);
      CHECK(bids[0].price == Catch::Approx{4487.66});
      CHECK(bids[0].size == Catch::Approx{34.16});
      CHECK(bids[8].price == Catch::Approx{4474.89});
      CHECK(bids[8].size == Catch::Approx{92.87});
      CHECK(data.checksum == 1990051938);
      CHECK(data.pseq == 1352182127478939650);
      CHECK(data.seq == 1352182127898370048);
      CHECK(data.ts == 1758111262297ms);
      CHECK(book.ts == 1758111262300ms);
      */
    }
    void operator()(Trace<json::Trades> const &) { FAIL(); }
    void operator()(Trace<json::Market24h> const &) { FAIL(); }
    void operator()(Trace<json::Kline> const &) { FAIL(); }
    void operator()(Trace<json::Login> const &) { FAIL(); }
    void operator()(Trace<json::Account> const &) { FAIL(); }
    void operator()(Trace<json::Position> const &) { FAIL(); }
    void operator()(Trace<json::Order> const &) { FAIL(); }
    void operator()(Trace<json::Fill> const &) { FAIL(); }
    size_t count = 0;
  } handler;
  TraceInfo trace_info;
  json::Parser::dispatch(handler, message, buffer_stack, trace_info, false);
  CHECK(handler.count == 1);
}
