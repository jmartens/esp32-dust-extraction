#include <cstdlib>

#include "unity.h"

extern "C" void app_main(void) {
  UNITY_BEGIN();
  unity_run_all_tests();
  const int failures = UNITY_END();
  std::exit(failures == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
}
