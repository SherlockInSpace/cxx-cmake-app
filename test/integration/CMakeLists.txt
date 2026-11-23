# SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
# SPDX-License-Identifier: MIT

# The integration tier: pytest runs the built application the way a user
# does and checks what it prints. One CTest test, labelled integration, so
# ctest -L integration runs the whole suite and pytest writes its JUnit file
# next to this directory's build output. A cross build cannot run the
# application on the build host, so the tier is only added for native builds.
if(CMAKE_CROSSCOMPILING)
  return()
endif()

find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)
execute_process(
  COMMAND ${Python3_EXECUTABLE} -c "import pytest"
  RESULT_VARIABLE pytest_missing
  OUTPUT_QUIET ERROR_QUIET)
if(pytest_missing)
  message(FATAL_ERROR "The integration tier needs pytest for ${Python3_EXECUTABLE}; "
    "the container image has it, or configure with -DBUILD_TESTING=OFF")
endif()

add_test(NAME integration
  COMMAND ${Python3_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
    --junitxml=${CMAKE_CURRENT_BINARY_DIR}/junit.xml
    -p no:cacheprovider -q
)
# pytest's cache and bytecode would otherwise land in the source tree.
set_tests_properties(integration PROPERTIES
  LABELS integration
  ENVIRONMENT "APP_BINARY=$<TARGET_FILE:app>;PYTHONDONTWRITEBYTECODE=1"
)
