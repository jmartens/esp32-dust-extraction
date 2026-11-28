#pragma once

#include <string_view>

#include "open_extraction/machine_demand_source/machine_demand_source.hpp"

namespace open_extraction::controller_configuration_service {

struct MachineUnitProfile {
  machine_demand_source::CalibrationState calibration;
};

enum class ProfileStorageResult { success, not_found, error };

class IMachineUnitProfileStorage {
public:
  virtual ~IMachineUnitProfileStorage() = default;

  // Implementations must preserve saved profiles across Controller restarts.
  [[nodiscard]] virtual ProfileStorageResult
  load(std::string_view machine_unit_id, MachineUnitProfile &profile) const = 0;
  [[nodiscard]] virtual ProfileStorageResult
  save(std::string_view machine_unit_id, const MachineUnitProfile &profile) = 0;
};

enum class ConfigurationResult {
  success,
  not_found,
  invalid_argument,
  invalid_profile,
  storage_error
};

[[nodiscard]] bool is_valid(const MachineUnitProfile &profile) noexcept;

class ControllerConfigurationService final {
public:
  explicit ControllerConfigurationService(IMachineUnitProfileStorage &storage);

  [[nodiscard]] ConfigurationResult
  save_machine_unit_profile(std::string_view machine_unit_id,
                            const MachineUnitProfile &profile);
  [[nodiscard]] ConfigurationResult
  load_machine_unit_profile(std::string_view machine_unit_id,
                            MachineUnitProfile &profile) const;
  [[nodiscard]] ConfigurationResult apply_machine_unit_profile(
      std::string_view machine_unit_id,
      machine_demand_source::IMachineDemandSource &demand_source) const;

private:
  IMachineUnitProfileStorage &storage_;
};

} // namespace open_extraction::controller_configuration_service
