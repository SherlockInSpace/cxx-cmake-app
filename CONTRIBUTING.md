# Contributing

This file describes how changes land in this repository and in any project
created from it. Rename `app` to your application's name as you read.

## From issue to merge

1. **Start from an issue.** Every change has one. Its title is a Conventional
   Commit subject (`fix(app): ...`, `build(cmake): ...`) and its body ends in a
   "Done when" list.
2. **Branch** as `issue/<number>-<short-slug>` from `main`.
3. **Commit** in Conventional Commit style. The subject is the issue title, the
   body says what changed and why, and the issue goes in the body:
   `Closes #N` when the commit finishes it, `Refs #N` when it does not. Every
   commit builds and passes the tests on its own, because pull requests are
   rebase-merged and each commit lands as it was reviewed.
4. **Keep pull requests small**: one issue, one to a few commits. Say in the
   description what you verified and how, and what you could not.
5. **CI is the gate.** A pull request merges when its checks are green; local
   hooks are optional and nothing depends on them.

Releases are cut from the commit subjects: `fix:` is a patch, `feat:` a minor,
`!` a major (until 1.0, a minor). Choose the type for what the change means to
someone running the application.

## Definition of done

Run these from the repository root, in the `dev` image, before asking for
review. CI runs the same checks.

```sh
# Warnings are errors; unit and integration tiers. Configure also checks that
# every source and public header has a test file.
cmake --workflow --preset ci-debug

# The same under ASan and UBSan.
cmake --workflow --preset asan-ubsan

# Layout, static analysis and the Python.
clang-format --dry-run -Werror $(git ls-files 'src/*.cpp' 'src/*.hpp' 'inc/*.hpp' 'test/*.cpp')
run-clang-tidy -p build/ci-debug 'src/|test/unit/'
ruff check
```

A change to the zones or to `ENABLE_TRACY` also runs
`cmake --workflow --preset tracy`.

## Code

- C++23, the library template's camelCase as `.clang-tidy` encodes it.
- Logic goes in `app_core` (`src/`, with public headers in `inc/app/`), where the
  unit tier can test it; `main.cpp` only handles arguments and output.
- Results go to stdout and logs to stderr through spdlog, so the integration
  tier and anyone piping the output see only results.
- A new source file or header gets `test/unit/src/<stem>.cpp`, or an
  `APP_TEST_EXEMPT` entry with the reason. Behaviour a user sees gets a test in
  `test/integration/`.
- A new dependency is a `CPMAddPackage` with a minimum `VERSION`, a release
  `URL` and its `URL_HASH`, and goes into the container image too. Nothing may
  download at configure time by default.
- Every new file has the two SPDX lines, or an entry in `REUSE.toml`.
