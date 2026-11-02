#include "open_extraction/controller_configuration_service/controller_configuration_service.hpp"

#include <cmath>

namespace open_extraction::controller_configuration_service {
namespace {

[[nodiscard]] ConfigurationResult
to_configuration_result(const ProfileStorageResult result) {
  switch (result) {
  case ProfileStorageResult::success:
    return ConfigurationResult::success;
  case ProfileStorageResult::not_found:
    return ConfigurationResult::not_found;
  case ProfileStorageResult::error:
    return ConfigurationResult::storage_error;
  }
  return ConfigurationResult::storage_error;
}

} // namespace

bool is_valid(const MachineUnitProfile &profile) noexcept {
  return std::isfinite(profile.calibration.factor) &&
         profile.calibration.factor > 0.0F;
}

ControllerConfigurationService::ControllerConfigurationService(
    IMachineUnitProfileStorage &storage)
    : storage_(storage) {}

ConfigurationResult ControllerConfigurationService::save_machine_unit_profile(
    const std::string_view machine_unit_id, const MachineUnitProfile &profile) {
  if (machine_unit_id.empty()) {
    return ConfigurationResult::invalid_argument;
  }
  if (!is_valid(profile)) {
    return ConfigurationResult::invalid_profile;
  }
  return storage_.save(machine_unit_id, profile) ==
                 ProfileStorageResult::success
             ? ConfigurationResult::success
             : ConfigurationResult::storage_error;
}

ConfigurationResult ControllerConfigurationService::load_machine_unit_profile(
    const std::string_view machine_unit_id, MachineUnitProfile &profile) const {
  if (machine_unit_id.empty()) {
    return ConfigurationResult::invalid_argument;
  }

  MachineUnitProfile loaded{{false, 0.0F}};
  const ProfileStorageResult result = storage_.load(machine_unit_id, loaded);
  if (result != ProfileStorageResult::success) {
    return to_configuration_result(result);
  }
  if (!is_valid(loaded)) {
    return ConfigurationResult::invalid_profile;
  }

  profile = loaded;
  return ConfigurationResult::success;
}

ConfigurationResult ControllerConfigurationService::apply_machine_unit_profile(
    const std::string_view machine_unit_id,
    machine_demand_source::IMachineDemandSource &demand_source) const {
  MachineUnitProfile profile{};
  const ConfigurationResult result =
      load_machine_unit_profile(machine_unit_id, profile);
  if (result != ConfigurationResult::success) {
    return result;
  }

  demand_source.apply_calibration(profile.calibration);
  return ConfigurationResult::success;
}

} // namespace open_extraction::controller_configuration_service
