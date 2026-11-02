#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <string_view>

#include "open_extraction/controller_configuration_service/controller_configuration_service.hpp"
#include "open_extraction/machine_demand_source/machine_demand_source.hpp"
#include "unity.h"

namespace {

using open_extraction::controller_configuration_service::ConfigurationResult;
using open_extraction::controller_configuration_service::
    ControllerConfigurationService;
using open_extraction::controller_configuration_service::
    IMachineUnitProfileStorage;
using open_extraction::controller_configuration_service::MachineUnitProfile;
using open_extraction::controller_configuration_service::ProfileStorageResult;
using open_extraction::machine_demand_source::CalibrationState;
using open_extraction::machine_demand_source::IMachineDemandSource;
using open_extraction::machine_demand_source::MachineDemandDiagnostics;

class FakeMachineUnitProfileStorage final : public IMachineUnitProfileStorage {
public:
  [[nodiscard]] ProfileStorageResult
  load(const std::string_view machine_unit_id,
       MachineUnitProfile &profile) const override {
    ++load_calls;
    if (load_result != ProfileStorageResult::success) {
      return load_result;
    }

    const auto stored = profiles.find(std::string(machine_unit_id));
    if (stored == profiles.end()) {
      return ProfileStorageResult::not_found;
    }
    profile = stored->second;
    return ProfileStorageResult::success;
  }

  [[nodiscard]] ProfileStorageResult
  save(const std::string_view machine_unit_id,
       const MachineUnitProfile &profile) override {
    ++save_calls;
    if (save_result != ProfileStorageResult::success) {
      return save_result;
    }

    profiles[std::string(machine_unit_id)] = profile;
    return ProfileStorageResult::success;
  }

  mutable std::size_t load_calls{};
  std::size_t save_calls{};
  mutable ProfileStorageResult load_result{ProfileStorageResult::success};
  ProfileStorageResult save_result{ProfileStorageResult::success};
  std::map<std::string, MachineUnitProfile> profiles;
};

class FakeMachineDemandSource final : public IMachineDemandSource {
public:
  [[nodiscard]] bool demand_active() const override { return false; }
  [[nodiscard]] MachineDemandDiagnostics diagnostics() const override {
    return {};
  }
  [[nodiscard]] CalibrationState calibration() const override {
    return calibration_;
  }
  void apply_calibration(const CalibrationState &calibration) override {
    calibration_ = calibration;
    ++apply_calls;
  }

  [[nodiscard]] std::size_t calibration_applications() const {
    return apply_calls;
  }

private:
  CalibrationState calibration_{};
  std::size_t apply_calls{};
};

} // namespace

TEST_CASE("profile validation accepts positive finite factors for either state",
          "[controller_configuration_service]") {
  TEST_ASSERT_TRUE(open_extraction::controller_configuration_service::is_valid(
      MachineUnitProfile{{true, 2.5F}}));
  TEST_ASSERT_TRUE(open_extraction::controller_configuration_service::is_valid(
      MachineUnitProfile{{false, 1.0F}}));
  TEST_ASSERT_TRUE(open_extraction::controller_configuration_service::is_valid(
      MachineUnitProfile{{true, std::numeric_limits<float>::denorm_min()}}));
}

TEST_CASE("profile validation rejects nonpositive and nonfinite factors",
          "[controller_configuration_service]") {
  const float invalid_factors[] = {
      0.0F,
      -1.0F,
      std::numeric_limits<float>::quiet_NaN(),
      std::numeric_limits<float>::infinity(),
      -std::numeric_limits<float>::infinity(),
  };

  for (const float factor : invalid_factors) {
    TEST_ASSERT_FALSE(
        open_extraction::controller_configuration_service::is_valid(
            MachineUnitProfile{{true, factor}}));
  }
  TEST_ASSERT_FALSE(open_extraction::controller_configuration_service::is_valid(
      MachineUnitProfile{{false, 0.0F}}));
}

TEST_CASE("service saves valid calibration profiles independently per unit",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  ControllerConfigurationService service(storage);

  const ConfigurationResult first_result = service.save_machine_unit_profile(
      "saw", MachineUnitProfile{{true, 2.0F}});
  const ConfigurationResult second_result = service.save_machine_unit_profile(
      "planer", MachineUnitProfile{{true, 3.0F}});

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                        static_cast<int>(first_result));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                        static_cast<int>(second_result));
  TEST_ASSERT_EQUAL_size_t(2, storage.save_calls);
  TEST_ASSERT_TRUE(storage.profiles.at("saw").calibration ==
                   (CalibrationState{true, 2.0F}));
  TEST_ASSERT_TRUE(storage.profiles.at("planer").calibration ==
                   (CalibrationState{true, 3.0F}));
}

TEST_CASE("service rejects invalid profile and empty unit before storage",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  ControllerConfigurationService service(storage);
  const MachineUnitProfile valid_profile{{true, 2.0F}};
  const MachineUnitProfile invalid_profile{{true, 0.0F}};

  const ConfigurationResult empty_id =
      service.save_machine_unit_profile("", valid_profile);
  const ConfigurationResult invalid =
      service.save_machine_unit_profile("saw", invalid_profile);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_argument),
                        static_cast<int>(empty_id));
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_profile),
                        static_cast<int>(invalid));
  TEST_ASSERT_EQUAL_size_t(0, storage.save_calls);
  TEST_ASSERT_TRUE(storage.profiles.empty());
}

TEST_CASE("service reports storage failure when saving a valid profile",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.save_result = ProfileStorageResult::error;
  ControllerConfigurationService service(storage);

  const ConfigurationResult result = service.save_machine_unit_profile(
      "saw", MachineUnitProfile{{true, 2.0F}});

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::storage_error),
                        static_cast<int>(result));
  TEST_ASSERT_EQUAL_size_t(1, storage.save_calls);
  TEST_ASSERT_TRUE(storage.profiles.empty());
}

TEST_CASE("service loads the requested unit profile into the output",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.profiles.emplace("saw", MachineUnitProfile{{true, 2.0F}});
  storage.profiles.emplace("planer", MachineUnitProfile{{true, 3.0F}});
  ControllerConfigurationService service(storage);
  MachineUnitProfile loaded{{false, 1.0F}};

  const ConfigurationResult result =
      service.load_machine_unit_profile("planer", loaded);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(loaded.calibration == (CalibrationState{true, 3.0F}));
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("missing profile leaves caller output unchanged",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  ControllerConfigurationService service(storage);
  MachineUnitProfile loaded{{true, 2.5F}};

  const ConfigurationResult result =
      service.load_machine_unit_profile("missing", loaded);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::not_found),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(loaded.calibration == (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("storage load error leaves caller output unchanged",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.load_result = ProfileStorageResult::error;
  ControllerConfigurationService service(storage);
  MachineUnitProfile loaded{{true, 2.5F}};

  const ConfigurationResult result =
      service.load_machine_unit_profile("saw", loaded);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::storage_error),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(loaded.calibration == (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("invalid persisted profile is rejected without changing output",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.profiles.emplace("saw", MachineUnitProfile{{true, -1.0F}});
  ControllerConfigurationService service(storage);
  MachineUnitProfile loaded{{true, 2.5F}};

  const ConfigurationResult result =
      service.load_machine_unit_profile("saw", loaded);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_profile),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(loaded.calibration == (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("service applies persisted calibration for the requested unit",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.profiles.emplace("saw", MachineUnitProfile{{true, 2.5F}});
  storage.profiles.emplace("planer", MachineUnitProfile{{true, 4.0F}});
  ControllerConfigurationService service(storage);
  FakeMachineDemandSource demand_source;

  const ConfigurationResult result =
      service.apply_machine_unit_profile("saw", demand_source);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(demand_source.calibration() ==
                   (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("service recreation reapplies calibration from shared storage",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  {
    ControllerConfigurationService before_restart(storage);
    const ConfigurationResult saved = before_restart.save_machine_unit_profile(
        "saw", MachineUnitProfile{{true, 2.5F}});
    TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                          static_cast<int>(saved));
  }

  ControllerConfigurationService after_restart(storage);
  FakeMachineDemandSource demand_source;
  const ConfigurationResult applied =
      after_restart.apply_machine_unit_profile("saw", demand_source);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::success),
                        static_cast<int>(applied));
  TEST_ASSERT_TRUE(demand_source.calibration() ==
                   (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());
  TEST_ASSERT_EQUAL_size_t(1, storage.save_calls);
  TEST_ASSERT_EQUAL_size_t(1, storage.load_calls);
}

TEST_CASE("apply does not change demand source on missing invalid or storage "
          "errors",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  storage.profiles.emplace("invalid", MachineUnitProfile{{true, 0.0F}});
  ControllerConfigurationService service(storage);
  FakeMachineDemandSource demand_source;
  demand_source.apply_calibration({true, 1.5F});
  const CalibrationState initial_calibration = demand_source.calibration();

  const ConfigurationResult missing =
      service.apply_machine_unit_profile("missing", demand_source);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::not_found),
                        static_cast<int>(missing));
  TEST_ASSERT_TRUE(demand_source.calibration() == initial_calibration);
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());

  const ConfigurationResult invalid =
      service.apply_machine_unit_profile("invalid", demand_source);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_profile),
                        static_cast<int>(invalid));
  TEST_ASSERT_TRUE(demand_source.calibration() == initial_calibration);
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());

  storage.load_result = ProfileStorageResult::error;
  const ConfigurationResult storage_error =
      service.apply_machine_unit_profile("saw", demand_source);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::storage_error),
                        static_cast<int>(storage_error));
  TEST_ASSERT_TRUE(demand_source.calibration() == initial_calibration);
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());

  storage.load_result = ProfileStorageResult::success;
  const ConfigurationResult invalid_id =
      service.apply_machine_unit_profile("", demand_source);
  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_argument),
                        static_cast<int>(invalid_id));
  TEST_ASSERT_TRUE(demand_source.calibration() == initial_calibration);
  TEST_ASSERT_EQUAL_size_t(1, demand_source.calibration_applications());
}

TEST_CASE("service rejects empty unit id without reading storage",
          "[controller_configuration_service]") {
  FakeMachineUnitProfileStorage storage;
  ControllerConfigurationService service(storage);
  MachineUnitProfile loaded{{true, 2.5F}};

  const ConfigurationResult result =
      service.load_machine_unit_profile("", loaded);

  TEST_ASSERT_EQUAL_INT(static_cast<int>(ConfigurationResult::invalid_argument),
                        static_cast<int>(result));
  TEST_ASSERT_TRUE(loaded.calibration == (CalibrationState{true, 2.5F}));
  TEST_ASSERT_EQUAL_size_t(0, storage.load_calls);
}
