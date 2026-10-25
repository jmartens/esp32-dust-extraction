#include <string>
#include <string_view>

#include "unity.h"

#include "open_extraction/collector_api/collector_api.hpp"
#include "open_extraction/http_routes/http_routes.hpp"
#include "open_extraction/http_server/http_server.hpp"

namespace {

using open_extraction::collector_api::CollectorApi;
using open_extraction::collector_api::ICollectorControl;
using open_extraction::collector_unit::CollectorState;
using open_extraction::http_routes::make_request_handler;
using open_extraction::http_server::HttpServer;
using open_extraction::http_server::Method;
using open_extraction::http_server::Request;
using open_extraction::http_server::Response;

class FakeControl final : public ICollectorControl {
public:
  CollectorState current = CollectorState::stopped;
  bool accept = true;
  int starts = 0;
  int stops = 0;

  CollectorState state() const override { return current; }
  std::string device_uid() const override { return "collector-01"; }
  bool request_start() override {
    ++starts;
    return accept;
  }
  bool request_stop() override {
    ++stops;
    return accept;
  }
};

struct Fixture {
  FakeControl control;
  CollectorApi api{control};
  HttpServer server{{}, make_request_handler(api)};

  Response get(std::string_view path) {
    return server.handle({Method::get, path});
  }
  Response post(std::string_view path) {
    return server.handle({Method::post, path});
  }
};

} // namespace

TEST_CASE("GET / serves the static page as text/html", "[http_routes]") {
  Fixture f;
  const Response response = f.get("/");
  TEST_ASSERT_EQUAL_INT(200, response.status);
  TEST_ASSERT_EQUAL_STRING("text/html", response.content_type.c_str());
  TEST_ASSERT_TRUE(response.body.find("<!DOCTYPE html>") != std::string::npos);
}

TEST_CASE("the page route ignores a query string", "[http_routes]") {
  Fixture f;
  const Response response = f.get("/?x=1");
  TEST_ASSERT_EQUAL_INT(200, response.status);
  TEST_ASSERT_EQUAL_STRING("text/html", response.content_type.c_str());
}

TEST_CASE("POST / is method not allowed", "[http_routes]") {
  Fixture f;
  const Response response = f.post("/");
  TEST_ASSERT_EQUAL_INT(405, response.status);
  TEST_ASSERT_EQUAL_STRING("application/json", response.content_type.c_str());
}

TEST_CASE("GET collector state is forwarded as JSON", "[http_routes]") {
  Fixture f;
  f.control.current = CollectorState::running;
  const Response response = f.get("/api/v1/collector");
  TEST_ASSERT_EQUAL_INT(200, response.status);
  TEST_ASSERT_EQUAL_STRING("application/json", response.content_type.c_str());
  TEST_ASSERT_EQUAL_STRING(
      "{\"state\":\"running\",\"device_uid\":\"collector-01\"}",
      response.body.c_str());
}

TEST_CASE("POST start and stop are forwarded with 202", "[http_routes]") {
  Fixture f;
  TEST_ASSERT_EQUAL_INT(202, f.post("/api/v1/collector/start").status);
  TEST_ASSERT_EQUAL_INT(1, f.control.starts);
  TEST_ASSERT_EQUAL_INT(202, f.post("/api/v1/collector/stop").status);
  TEST_ASSERT_EQUAL_INT(1, f.control.stops);
}

TEST_CASE("API conflict, 404 and 405 responses are preserved",
          "[http_routes]") {
  Fixture f;
  f.control.accept = false;
  const Response conflict = f.post("/api/v1/collector/start");
  TEST_ASSERT_EQUAL_INT(409, conflict.status);
  TEST_ASSERT_EQUAL_STRING(
      "{\"error\":\"request not permitted in current state\"}",
      conflict.body.c_str());

  const Response unknown = f.get("/api/v1/unknown");
  TEST_ASSERT_EQUAL_INT(404, unknown.status);
  TEST_ASSERT_EQUAL_STRING("{\"error\":\"not found\"}", unknown.body.c_str());

  const Response wrong_method = f.get("/api/v1/collector/start");
  TEST_ASSERT_EQUAL_INT(405, wrong_method.status);
  TEST_ASSERT_EQUAL_INT(1, f.control.starts);
}

TEST_CASE("unknown non-API paths return a JSON 404", "[http_routes]") {
  Fixture f;
  for (const std::string_view path : {"/index.html", "/api", "/apix/v1"}) {
    const Response response = f.get(path);
    TEST_ASSERT_EQUAL_INT(404, response.status);
    TEST_ASSERT_EQUAL_STRING("application/json", response.content_type.c_str());
  }
}

TEST_CASE("unsupported methods are rejected by the server with 405",
          "[http_routes]") {
  Fixture f;
  const Response response =
      f.server.handle({Method::unsupported, "/api/v1/collector"});
  TEST_ASSERT_EQUAL_INT(405, response.status);
}
