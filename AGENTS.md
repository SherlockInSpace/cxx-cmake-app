# AGENTS.md

Instructions for coding agents working in this repository. People should read
[CONTRIBUTING.md](CONTRIBUTING.md); this is the short form of the same rules.

## Build and test

Always use the presets; never invent build directories or flags.

| Task | Command |
|---|---|
| Build and run every test | `cmake --workflow --preset dev` |
| What CI checks (warnings are errors) | `cmake --workflow --preset ci-debug` |
| Unit or integration tier only | `ctest --preset dev -L unit`, `ctest --preset dev -L integration` |
| Sanitizers | `cmake --workflow --preset asan-ubsan` |
| Tracy build | `cmake --workflow --preset tracy` |
| Static analysis | `run-clang-tidy -p build/dev 'src/\|test/unit/'` |
| Formatting | `clang-format -i <changed C++ files>` |
| Python lint | `ruff check` |
| Build against a library checkout | `cmake --preset dev -DCPM_util_SOURCE=/work/cxx-cmake-library` |

A change is done when `ci-debug` and `asan-ubsan` pass and clang-format,
clang-tidy and ruff report nothing.

## Rules

- Logic goes in `app_core`; keep `main.cpp` to arguments and output.
- Results on stdout, logs on stderr through spdlog.
- Every file in `src/` and header in `inc/app/` needs `test/unit/src/<stem>.cpp`.
- Dependencies are `CPMAddPackage` with a `URL_HASH`; nothing downloads at
  configure time by default.
- Tracy only through `APP_ZONE` and `APP_FRAME` from `src/profiling.hpp`.
- New files get SPDX headers (see `REUSE.toml` for files that cannot).
- Commits are Conventional Commits with the issue in the body (`Closes #N` or
  `Refs #N`); each commit must build and pass on its own.
