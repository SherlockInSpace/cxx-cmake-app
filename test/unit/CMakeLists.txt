# SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
# SPDX-License-Identifier: MIT

# One binary for all the unit tests, as in the library template.
# GTestConfig.cmake from the image or sysroot. If CPM is allowed to fetch,
# it builds the 1.17.0 release tarball, checked by its SHA-256.
CPMAddPackage(
  NAME GTest
  FORCE "${CPM_DOWNLOAD_GTest}"
  VERSION 1.14.0
  URL https://github.com/google/googletest/releases/download/v1.17.0/googletest-1.17.0.tar.gz
  URL_HASH SHA256=65fab701d9829d38cb77c14acdc431d2108bfdbf8979e40eb8ae567edf10b27c
  FIND_PACKAGE_ARGUMENTS "CONFIG"
  OPTIONS "BUILD_GMOCK OFF" "INSTALL_GTEST OFF"
)
include(GoogleTest)

# Every source in src/ and header in inc/app/ needs a test file of the same
# stem in test/unit/src/, so an untested module fails at configure. A stem
# with nothing to test on its own goes in APP_TEST_EXEMPT with the reason.
set(APP_TEST_EXEMPT
  # Argument handling and printing only; the integration tier runs it.
  main
)

foreach(file IN LISTS SOURCE_FILES PUBLIC_HEADER_FILES)
  cmake_path(GET file STEM stem)
  if(stem IN_LIST APP_TEST_EXEMPT)
    continue()
  endif()
  if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/${stem}.cpp")
    cmake_path(RELATIVE_PATH file BASE_DIRECTORY "${PROJECT_SOURCE_DIR}")
    message(FATAL_ERROR
      "${file} has no unit test: "
      "add test/unit/src/${stem}.cpp or list ${stem} in APP_TEST_EXEMPT")
  endif()
endforeach()

file(GLOB UNIT_TEST_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(app_unit_tests ${UNIT_TEST_FILES})
target_link_libraries(app_unit_tests
  PRIVATE app_core app_warnings GTest::gtest_main
)
set_target_properties(app_unit_tests PROPERTIES CXX_SCAN_FOR_MODULES OFF)

# PRE_TEST lists the tests when ctest runs, so a cross build never has to
# run the binary on the build host.
gtest_discover_tests(app_unit_tests
  DISCOVERY_MODE PRE_TEST
  PROPERTIES LABELS unit
)
