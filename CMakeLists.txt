# SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 4.3)

################################################################################
# Project name and configuration
project(
  "app"
  VERSION 0.1.0
  DESCRIPTION
    "Example C++ CMake application."
  HOMEPAGE_URL ""
  LANGUAGES CXX)

################################################################################
# Do NOT allow in-source builds
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. "
      "Please make a build directory and run CMake from there."
  )
endif()

################################################################################
# Includes
include(CTest)
include(GNUInstallDirs)

################################################################################
# Dependencies
# As in the library template: every dependency resolves through find_package,
# from the container image or the Yocto sysroot, and configure fails if one is
# missing rather than downloading it. -DCPM_LOCAL_PACKAGES_ONLY=OFF lets CPM
# fetch what find_package cannot find.
option(CPM_USE_LOCAL_PACKAGES "Try find_package before downloading" ON)
option(CPM_LOCAL_PACKAGES_ONLY "Only use find_package, never download" ON)
include(${PROJECT_SOURCE_DIR}/cmake/CPM.cmake)

# CPM reports a missing package with SEND_ERROR and then tries to download it;
# with FetchContent disconnected that cannot happen.
if(CPM_LOCAL_PACKAGES_ONLY)
  set(FETCHCONTENT_FULLY_DISCONNECTED ON)
endif()

# util, the library template: its latest release from the image, or the
# Yocto sysroot. For a change that spans both repositories,
# -DCPM_util_SOURCE=/work/cxx-cmake-library builds a library checkout instead
# (README "Changes that span repositories" in the library). If CPM is allowed
# to fetch, it builds the v0.3.0 tag's tarball, checked by its SHA-256.
CPMAddPackage(
  NAME util
  FORCE "${CPM_DOWNLOAD_util}"
  VERSION 0.3.0
  URL https://github.com/SherlockInSpace/cxx-cmake-library/archive/refs/tags/v0.3.0.tar.gz
  URL_HASH SHA256=e8d0141614fb14a78f2b6976e60dcfc233a135c8fe64b36b80babac6b6478b6c
  FIND_PACKAGE_ARGUMENTS "CONFIG"
  OPTIONS "BUILD_TESTING OFF"
)

# spdlog from the image or the Yocto sysroot. If CPM is allowed to fetch
# (-DCPM_LOCAL_PACKAGES_ONLY=OFF), it builds the v1.17.0 source tarball,
# checked by its SHA-256; -DCPM_DOWNLOAD_spdlog=ON builds that even when a
# local spdlog exists. CPM only reads that switch in CPMFindPackage, so it is
# passed through as FORCE.
CPMAddPackage(
  NAME spdlog
  FORCE "${CPM_DOWNLOAD_spdlog}"
  VERSION 1.15.0
  URL https://github.com/gabime/spdlog/archive/refs/tags/v1.17.0.tar.gz
  URL_HASH SHA256=d8862955c6d74e5846b3f580b1605d2428b11d97a410d86e2fb13e857cd3a744
  FIND_PACKAGE_ARGUMENTS "CONFIG"
  OPTIONS "SPDLOG_INSTALL OFF" "SPDLOG_BUILD_EXAMPLE OFF"
)

################################################################################
# Warnings
# On app_warnings, an INTERFACE target the app links PRIVATE. -Werror is a
# separate switch: on in the CI presets, off in dev.
option(APP_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(app_warnings INTERFACE)
target_compile_options(app_warnings INTERFACE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:
    -Wall -Wextra -Wpedantic
    -Wshadow -Wconversion -Wsign-conversion -Wold-style-cast -Wcast-align
    -Wnon-virtual-dtor -Woverloaded-virtual -Wnull-dereference
    -Wdouble-promotion -Wformat=2 -Wimplicit-fallthrough>
  $<$<CXX_COMPILER_ID:GNU>:
    -Wduplicated-cond -Wduplicated-branches -Wlogical-op -Wuseless-cast>
  $<$<AND:$<BOOL:${APP_WARNINGS_AS_ERRORS}>,$<CXX_COMPILER_ID:GNU,Clang,AppleClang>>:-Werror>
)

################################################################################
# The application
# Sources are globbed, so a new file needs no edit here. CONFIGURE_DEPENDS
# re-checks the glob at build time.
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_23)
# No C++20 modules, so skip the module scan (it also puts GCC-only flags into
# compile_commands.json that clang-tidy rejects).
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES OFF)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    app_warnings
    util::util
    spdlog::spdlog
)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

################################################################################
# Tests
# Until the unit and integration tiers exist, one check that the binary runs.
if(BUILD_TESTING)
  add_test(NAME app.runs COMMAND ${PROJECT_NAME} alpha beta gamma)
  set_tests_properties(app.runs PROPERTIES LABELS functional)
endif()
