#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <span>
#include <string>

#include "esp_log.h"
#include "esp_mac.h"
#include "sdkconfig.h"

#include "open_extraction/collector_api/collector_api.hpp"
#include "open_extraction/collector_commands/start_collector.hpp"
#include "open_extraction/collector_commands/stop_collector.hpp"
#include "open_extraction/collector_service/collector_service.hpp"
#include "open_extraction/collector_state_machine/collector_state_machine.hpp"
#include "open_extraction/command_dispatcher/command_dispatcher.hpp"
#include "open_extraction/event_id_generator/esp_event_id_generator.hpp"
#include "open_extraction/http_routes/http_routes.hpp"
#include "open_extraction/http_server/http_server.hpp"

namespace {

using namespace open_extraction;

constexpr const char *kTag = "app_main";

// No hardware-backed collector exists yet, so the unit follows the state
// machine and confirms transitions immediately.
class StateMachineUnit final : public collector_unit::ICollectorUnit {
public:
  bool start() override {
    return machine_.request_start() && machine_.confirm_started();
  }
  bool stop() override {
    return machine_.request_stop() && machine_.confirm_stopped();
  }
  collector_unit::CollectorState state() const override {
    return machine_.state();
  }
  collector_unit::CollectorDiagnostics diagnostics() const override {
    return {};
  }
  std::span<const collector_unit::CollectorCapability>
  capabilities() const override {
    return kCapabilities;
  }

private:
  static constexpr std::array kCapabilities = {
      collector_unit::CollectorCapability::start_stop};
  collector_state_machine::CollectorStateMachine machine_;
};

std::string device_uid_string() {
  std::array<std::uint8_t, 6> mac{};
  (void)esp_read_mac(mac.data(), ESP_MAC_WIFI_STA);
  char text[32];
  std::snprintf(text, sizeof(text), "COLLECTOR-%02X%02X%02X%02X%02X%02X",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return text;
}

} // namespace

extern "C" void app_main(void) {
  static StateMachineUnit unit;
  static command_dispatcher::CommandDispatcher dispatcher;
  static event_id_generator::EspEventIdGenerator ids;

  const auto uid = event::DeviceUid::from_string(device_uid_string());
  if (!uid) {
    ESP_LOGE(kTag, "invalid device uid");
    return;
  }

  const bool registered =
      dispatcher.register_recipient(*uid, [](const command::Command &command) {
        if (command.command_type() ==
            collector_commands::StartCollector::kCommandType) {
          return unit.start();
        }
        if (command.command_type() ==
            collector_commands::StopCollector::kCommandType) {
          return unit.stop();
        }
        return false;
      });
  if (!registered) {
    ESP_LOGE(kTag, "failed to register collector command recipient");
    return;
  }

  static collector_service::CollectorService service(
      dispatcher, unit, *uid, ids, [] {
        return std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now());
      });
  static collector_api::CollectorApi api(service);
  static http_server::HttpServer server(
      {.port = static_cast<std::uint16_t>(CONFIG_OPEN_EXTRACTION_HTTP_PORT)},
      http_routes::make_request_handler(api));

  const std::string error = server.start();
  if (!error.empty()) {
    ESP_LOGE(kTag, "HTTP server failed to start: %s", error.c_str());
    return;
  }
  ESP_LOGI(kTag, "HTTP server listening on port %u",
           static_cast<unsigned>(server.port()));
}