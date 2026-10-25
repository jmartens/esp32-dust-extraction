cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")
set(COMPONENTS main)

option(OPEN_EXTRACTION_COVERAGE "Instrument host tests for gcov coverage" OFF)
if(OPEN_EXTRACTION_COVERAGE)
    idf_build_set_property(COMPILE_OPTIONS "--coverage" APPEND)
    idf_build_set_property(LINK_OPTIONS "--coverage" APPEND)
endif()

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(open_extraction_tests)
