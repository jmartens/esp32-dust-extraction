# The page lives in web/index.html and is compiled into the firmware by
# generating a translation unit at configure time. This works on every target,
# including the Linux host target used by the unit tests.
set(page_source "${CMAKE_CURRENT_LIST_DIR}/web/index.html")
set(page_generated "${CMAKE_CURRENT_BINARY_DIR}/collector_page_html.cpp")

set_property(
    DIRECTORY
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${page_source}"
)

file(READ "${page_source}" page_html)
file(
    CONFIGURE
    OUTPUT "${page_generated}"
    CONTENT
        [=[
#include "open_extraction/collector_page/collector_page.hpp"

namespace open_extraction::collector_page {

namespace {

constexpr std::string_view kPage = R"html(@page_html@)html";

} // namespace

std::string_view html() noexcept { return kPage; }

} // namespace open_extraction::collector_page
]=]
    @ONLY
)

idf_component_register(
    SRCS "${page_generated}"
    INCLUDE_DIRS "include"
)
