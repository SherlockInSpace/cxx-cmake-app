// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

// Tracy zones for the application. With -DENABLE_TRACY=ON they are Tracy's
// ZoneScoped and FrameMark; otherwise they compile to nothing and Tracy is not
// even a dependency. Write APP_ZONE; at the top of a scope to time it under the
// function's name, and APP_FRAME; where one round of work ends.

#pragma once

// Zones have to be macros: a zone records its source location and scope.
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#ifdef APP_ENABLE_TRACY
#include <tracy/Tracy.hpp>
#define APP_ZONE ZoneScoped
#define APP_FRAME FrameMark
#else
#define APP_ZONE static_cast<void>(0)
#define APP_FRAME static_cast<void>(0)
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)
