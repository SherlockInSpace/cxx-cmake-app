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
# app_core holds everything except main(), so the unit tests can link it;
# the app executable is main.cpp on top. Sources and headers are globbed, so a
# new file needs no edit here, and the test-file check in test/unit picks it
# up. CONFIGURE_DEPENDS re-checks the globs at build time.
file(GLOB PUBLIC_HEADER_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/inc/app/*.hpp
)
file(GLOB SOURCE_FILES CONFIGURE_DEPENDS
  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)
set(CORE_SOURCE_FILES ${SOURCE_FILES})
list(FILTER CORE_SOURCE_FILES EXCLUDE REGEX "/main\\.cpp$")

add_library(app_core STATIC)
target_sources(app_core
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc
    FILES ${PUBLIC_HEADER_FILES}
  PRIVATE
    ${CORE_SOURCE_FILES}
)
target_compile_features(app_core PUBLIC cxx_std_23)
target_link_libraries(app_core
  PUBLIC util::util
  PRIVATE app_warnings
)

# Tracy profiling, off by default. The zones in src/profiling.hpp compile to
# nothing unless this is on, and then Tracy::TracyClient comes from the image
# (or, if CPM may fetch, the v0.15.0 tarball checked by its SHA-256). An
# application is the final binary, so whether to ship one with zones is its
# own decision; the library template never has them.
option(ENABLE_TRACY "Build the application with Tracy profiling zones" OFF)

if(ENABLE_TRACY)
  CPMAddPackage(
    NAME Tracy
    FORCE "${CPM_DOWNLOAD_Tracy}"
    VERSION 0.15.0
    URL https://github.com/wolfpld/tracy/archive/refs/tags/v0.15.0.tar.gz
    URL_HASH SHA256=3d57da54aa67d7875da1e8fecd0bd5957675cef73c27dd52bffb6ab8d290a5e6
    FIND_PACKAGE_ARGUMENTS "CONFIG"
    OPTIONS "TRACY_ENABLE ON"
  )
  target_link_libraries(app_core PUBLIC Tracy::TracyClient)
  target_compile_definitions(app_core PUBLIC APP_ENABLE_TRACY)
endif()

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME}
  PRIVATE
    app_core
    app_warnings
    spdlog::spdlog
)

# No C++20 modules, so skip the module scan (it also puts GCC-only flags into
# compile_commands.json that clang-tidy rejects).
set_target_properties(app_core ${PROJECT_NAME} PROPERTIES CXX_SCAN_FOR_MODULES OFF)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

################################################################################
# Tests
# The unit tier (test/unit, GoogleTest) and the integration tier
# (test/integration, pytest driving the binary).
if(BUILD_TESTING)
  add_subdirectory(test)
endif()
