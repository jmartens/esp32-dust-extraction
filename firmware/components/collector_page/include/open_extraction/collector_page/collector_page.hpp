#pragma once

#include <string_view>

namespace open_extraction::collector_page {

// Self-contained HTML page (no external assets) for manual collector control.
// It only consumes the /api/v1/collector API and holds no collector logic.
[[nodiscard]] std::string_view html() noexcept;

} // namespace open_extraction::collector_page
