#include <string>

#include "unity.h"

#include "open_extraction/collector_api/collector_api.hpp"

namespace {

using open_extraction::collector_api::CollectorApi;
using open_extraction::collector_api::ICollectorControl;
using open_extraction::collector_api::Method;
using open_extraction::collector_api::Response;
using open_extraction::collector_unit::CollectorMode;
using open_extraction::collector_unit::CollectorState;

class FakeControl final : public ICollectorControl {
public:
  CollectorState current = CollectorState::stopped;
  CollectorMode current_mode = CollectorMode::automatic;
  bool active_demand = false;
  bool suppressed_demand = false;
  bool accept = true;
  int starts = 0;
  int stops = 0;
  int mode_requests = 0;

  CollectorState state() const override { return current; }
  CollectorMode mode() const override { return current_mode; }
  std::string device_uid() const override { return "collector-01"; }
  bool demand_active() const override { return active_demand; }
  bool demand_suppressed() const override { return suppressed_demand; }
  bool request_start() override {
    ++starts;
    return accept;
  }
  bool request_stop() override {
    ++stops;
    return accept;
  }
  bool request_mode(CollectorMode requested) override {
    ++mode_requests;
    if (accept) {
      current_mode = requested;
    }
    return accept;
  }
};

} // namespace

TEST_CASE("GET collector returns state and device uid as JSON",
          "[collector_api]") {
  FakeControl control;
  control.current = CollectorState::running;
  control.current_mode = CollectorMode::forced_off;
  control.active_demand = true;
  control.suppressed_demand = true;
  const Response r =
      CollectorApi(control).handle(Method::get, "/api/v1/collector");

  TEST_ASSERT_EQUAL_INT(200, r.status);
  TEST_ASSERT_EQUAL_STRING("{\"state\":\"running\",\"device_uid\":"
                           "\"collector-01\",\"mode\":\"FORCED_OFF\","
                           "\"demand_active\":true,"
                           "\"demand_suppressed\":true}",
                           r.body.c_str());
}

TEST_CASE(
    "GET collector exposes automatic mode and inactive unsuppressed demand",
    "[collector_api]") {
  FakeControl control;
  const Response r =
      CollectorApi(control).handle(Method::get, "/api/v1/collector");

  TEST_ASSERT_EQUAL_INT(200, r.status);
  TEST_ASSERT_EQUAL_STRING(
      "{\"state\":\"stopped\",\"device_uid\":\"collector-01\","
      "\"mode\":\"AUTO\",\"demand_active\":false,"
      "\"demand_suppressed\":false}",
      r.body.c_str());
}

TEST_CASE("POST start requests a start and returns 202", "[collector_api]") {
  FakeControl control;
  const Response r =
      CollectorApi(control).handle(Method::post, "/api/v1/collector/start");

  TEST_ASSERT_EQUAL_INT(202, r.status);
  TEST_ASSERT_EQUAL_INT(1, control.starts);
  TEST_ASSERT_EQUAL_INT(0, control.stops);
  TEST_ASSERT_NOT_EQUAL(std::string::npos, r.body.find("\"state\""));
}

TEST_CASE("POST stop requests a stop and returns 202", "[collector_api]") {
  FakeControl control;
  const Response r =
      CollectorApi(control).handle(Method::post, "/api/v1/collector/stop");

  TEST_ASSERT_EQUAL_INT(202, r.status);
  TEST_ASSERT_EQUAL_INT(1, control.stops);
  TEST_ASSERT_EQUAL_INT(0, control.starts);
}

TEST_CASE("a rejected request returns 409", "[collector_api]") {
  FakeControl control;
  control.accept = false;
  const Response r =
      CollectorApi(control).handle(Method::post, "/api/v1/collector/start");

  TEST_ASSERT_EQUAL_INT(409, r.status);
  TEST_ASSERT_NOT_EQUAL(std::string::npos, r.body.find("\"error\""));
}

TEST_CASE("wrong methods return 405 without issuing commands",
          "[collector_api]") {
  FakeControl control;
  const CollectorApi api(control);

  TEST_ASSERT_EQUAL_INT(
      405, api.handle(Method::get, "/api/v1/collector/start").status);
  TEST_ASSERT_EQUAL_INT(405,
                        api.handle(Method::post, "/api/v1/collector").status);
  TEST_ASSERT_EQUAL_INT(0, control.starts);
  TEST_ASSERT_EQUAL_INT(0, control.stops);
}

TEST_CASE("mode POST routes select and expose each supported mode",
          "[collector_api]") {
  FakeControl control;
  const CollectorApi api(control);

  const Response automatic =
      api.handle(Method::post, "/api/v1/collector/mode/auto");
  TEST_ASSERT_EQUAL_INT(202, automatic.status);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::automatic),
                        static_cast<int>(control.current_mode));
  TEST_ASSERT_NOT_EQUAL(std::string::npos,
                        automatic.body.find("\"mode\":\"AUTO\""));

  const Response forced_on =
      api.handle(Method::post, "/api/v1/collector/mode/forced-on");
  TEST_ASSERT_EQUAL_INT(202, forced_on.status);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_on),
                        static_cast<int>(control.current_mode));
  TEST_ASSERT_NOT_EQUAL(std::string::npos,
                        forced_on.body.find("\"mode\":\"FORCED_ON\""));

  const Response forced_off =
      api.handle(Method::post, "/api/v1/collector/mode/forced-off");
  TEST_ASSERT_EQUAL_INT(202, forced_off.status);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::forced_off),
                        static_cast<int>(control.current_mode));
  TEST_ASSERT_NOT_EQUAL(std::string::npos,
                        forced_off.body.find("\"mode\":\"FORCED_OFF\""));
  TEST_ASSERT_EQUAL_INT(3, control.mode_requests);
}

TEST_CASE("invalid mode paths and wrong mode methods do not mutate mode",
          "[collector_api]") {
  FakeControl control;
  const CollectorApi api(control);

  TEST_ASSERT_EQUAL_INT(
      405, api.handle(Method::get, "/api/v1/collector/mode/forced-on").status);
  TEST_ASSERT_EQUAL_INT(
      404, api.handle(Method::post, "/api/v1/collector/mode/unknown").status);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::automatic),
                        static_cast<int>(control.current_mode));
  TEST_ASSERT_EQUAL_INT(0, control.mode_requests);
}

TEST_CASE("rejected mode request returns conflict without changing mode",
          "[collector_api]") {
  FakeControl control;
  control.accept = false;

  const Response r = CollectorApi(control).handle(
      Method::post, "/api/v1/collector/mode/forced-off");

  TEST_ASSERT_EQUAL_INT(409, r.status);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(CollectorMode::automatic),
                        static_cast<int>(control.current_mode));
  TEST_ASSERT_EQUAL_INT(1, control.mode_requests);
  TEST_ASSERT_NOT_EQUAL(std::string::npos, r.body.find("\"error\""));
}

TEST_CASE("unknown paths return 404", "[collector_api]") {
  FakeControl control;
  const Response r =
      CollectorApi(control).handle(Method::get, "/api/v1/unknown");

  TEST_ASSERT_EQUAL_INT(404, r.status);
}

TEST_CASE("every collector state has a distinct name", "[collector_api]") {
  using open_extraction::collector_api::state_name;
  TEST_ASSERT_EQUAL_STRING(
      "faulted", std::string(state_name(CollectorState::faulted)).c_str());
  TEST_ASSERT_EQUAL_STRING(
      "stopping", std::string(state_name(CollectorState::stopping)).c_str());
  TEST_ASSERT_EQUAL_STRING(
      "starting", std::string(state_name(CollectorState::starting)).c_str());
}
