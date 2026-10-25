#include <string_view>

#include "unity.h"

#include "open_extraction/collector_page/collector_page.hpp"

namespace {

bool contains(std::string_view needle) {
  return open_extraction::collector_page::html().find(needle) !=
         std::string_view::npos;
}

} // namespace

TEST_CASE("the collector page is a complete HTML document",
          "[collector_page]") {
  TEST_ASSERT_TRUE(contains("<!DOCTYPE html>"));
  TEST_ASSERT_TRUE(contains("</html>"));
}

TEST_CASE("the collector page is usable on mobile browsers",
          "[collector_page]") {
  TEST_ASSERT_TRUE(contains("name=\"viewport\""));
  TEST_ASSERT_TRUE(contains("width=device-width"));
}

TEST_CASE("the collector page displays state and device uid",
          "[collector_page]") {
  TEST_ASSERT_TRUE(contains("id=\"state\""));
  TEST_ASSERT_TRUE(contains("id=\"device-uid\""));
}

TEST_CASE("the collector page offers start and stop through the API",
          "[collector_page]") {
  TEST_ASSERT_TRUE(contains("id=\"start\""));
  TEST_ASSERT_TRUE(contains("id=\"stop\""));
  TEST_ASSERT_TRUE(contains("/api/v1/collector"));
  TEST_ASSERT_TRUE(contains("\"/start\""));
  TEST_ASSERT_TRUE(contains("\"/stop\""));
}

TEST_CASE("the collector page loads no external assets", "[collector_page]") {
  TEST_ASSERT_FALSE(contains("http://"));
  TEST_ASSERT_FALSE(contains("https://"));
}
