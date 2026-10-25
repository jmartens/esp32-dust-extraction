#include "unity.h"

#include "open_extraction/http_server/http_server.hpp"

namespace {

using open_extraction::http_server::Config;
using open_extraction::http_server::HttpServer;
using open_extraction::http_server::Method;
using open_extraction::http_server::Request;
using open_extraction::http_server::Response;

} // namespace

TEST_CASE("HTTP server preserves its configured port", "[http_server]") {
  HttpServer server(Config{.port = 8080},
                    [](const Request &) { return Response{200, "ok"}; });
  TEST_ASSERT_EQUAL_INT(8080, server.port());
}

TEST_CASE("HTTP server forwards GET requests to its handler", "[http_server]") {
  bool called = false;
  HttpServer server(Config{}, [&called](const Request &request) {
    called = request.method == Method::get && request.path == "/status";
    return Response{200, "{\"state\":\"stopped\"}"};
  });

  const Response response = server.handle({Method::get, "/status"});

  TEST_ASSERT_TRUE(called);
  TEST_ASSERT_EQUAL_INT(200, response.status);
  TEST_ASSERT_EQUAL_STRING("{\"state\":\"stopped\"}", response.body.c_str());
}

TEST_CASE("HTTP server forwards POST requests to its handler",
          "[http_server]") {
  bool called = false;
  HttpServer server(Config{}, [&called](const Request &request) {
    called = request.method == Method::post && request.path == "/start";
    return Response{202, "{}"};
  });

  const Response response = server.handle({Method::post, "/start"});

  TEST_ASSERT_TRUE(called);
  TEST_ASSERT_EQUAL_INT(202, response.status);
}

TEST_CASE("HTTP server rejects unsupported methods", "[http_server]") {
  bool called = false;
  HttpServer server(Config{}, [&called](const Request &) {
    called = true;
    return Response{200, "{}"};
  });

  const Response response = server.handle({Method::unsupported, "/status"});

  TEST_ASSERT_FALSE(called);
  TEST_ASSERT_EQUAL_INT(405, response.status);
}

TEST_CASE("HTTP server reports that the host target cannot start it",
          "[http_server]") {
  HttpServer server(Config{},
                    [](const Request &) { return Response{200, ""}; });

  TEST_ASSERT_FALSE(server.start().empty());
  TEST_ASSERT_TRUE(server.stop().empty());
}
