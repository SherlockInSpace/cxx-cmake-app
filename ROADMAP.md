# cxx-cmake-app roadmap

This repo's slice of the family roadmap. The master decision log and full roadmap live in
the library repo (`docs/DECISIONS.md` / `docs/ROADMAP.md` once graduated there).

## Phase 5 (blocked on: library template complete through Phase 3; container images published)

1. Seed the application from the original monorepo's `app/` (fixing its `argv[1]`-in-loop
   bug), restructured to the family conventions: C++23, CMake ≥ 4.3, presets,
   camelCase/clang-tidy, warnings target.
2. Dependencies via CPM (`CPM_USE_LOCAL_PACKAGES=ON` + `CPM_LOCAL_PACKAGES_ONLY=ON`
   defaults): spdlog as the worked third-party example; the library consumed from the
   container baseline with the `CPM_<NAME>_SOURCE` local-checkout override documented.
3. Optional Tracy instrumentation behind `ENABLE_TRACY` (default OFF) — in-app zones only;
   headless capture flow documented for on-target profiling.
4. `integration` test tier: pytest driving the built binary as a subprocess, registered
   under CTest (`--junitxml` into the same report pipeline), ruff linting the Python.
5. CI skeleton shared with the library repo (reusable workflows): PR gate, post-merge full
   suite, weekly drift cron, release-please.
