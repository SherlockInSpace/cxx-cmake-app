# cxx-cmake-app

A template for C++ applications built with CMake: C++23, CMake 4.3, the
[library template](https://github.com/SherlockInSpace/cxx-cmake-library)'s
`util` and third-party libraries declared through CPM but resolved from the
build environment, a GoogleTest unit tier, a pytest integration tier that runs
the real binary, and optional Tracy profiling. The example application, `app`,
prints each word it is given as `new` or `seen`, using `util`'s Bloom filter;
replace it with your own and keep the rest.

It is one of four repositories:

- [cxx-cmake-container](https://github.com/SherlockInSpace/cxx-cmake-container)
  builds the images every build runs in, with `util`, spdlog and Tracy installed;
- [cxx-cmake-library](https://github.com/SherlockInSpace/cxx-cmake-library),
  the library template, whose CI workflows this repository calls;
- this repository, the application template;
- [meta-cxx-cmake](https://github.com/SherlockInSpace/meta-cxx-cmake), the Yocto
  layer that packages both for the board.

## Quick start

Builds run inside the `dev` image. From the directory above your checkouts:

```sh
docker run --rm -it -e HOST_UID=$(id -u) -e HOST_GID=$(id -g) \
  -v "$PWD":/work -v cxx-cmake-ccache:/home/dev/.cache/ccache \
  ghcr.io/sherlockinspace/cxx-cmake-container/dev:0.1.5@sha256:e8663a5cd899b57203559db7e49b05b2fa29fc2736809906a6f21bbb767aadc3
```

or open the checkout in an editor that supports Dev Containers
(`.devcontainer/devcontainer.json` starts the same image). Then:

```sh
cd /work/cxx-cmake-app
cmake --workflow --preset dev
build/dev/app alpha beta alpha
```

The workflow configures, builds and runs every test; the last line prints
`new alpha`, `new beta`, `seen alpha`. `SPDLOG_LEVEL=debug` adds a log line per
word on stderr.

## Presets

| Preset | What it is |
|---|---|
| `dev` | Debug, warnings shown but not fatal. |
| `release` | Release. |
| `ci-debug`, `ci-release` | What CI builds: warnings are errors. |
| `asan-ubsan` | Debug under AddressSanitizer and UBSan. |
| `tracy` | RelWithDebInfo with `ENABLE_TRACY` (see Profiling). |

Each has a workflow preset, so `cmake --workflow --preset <name>` configures,
builds and tests; build trees go in `build/<preset>`.

## Dependencies

`util`, spdlog, GoogleTest and Tracy are each a `CPMAddPackage` in
`CMakeLists.txt` or `test/unit/CMakeLists.txt`. By default every one resolves
with `find_package` from the image (or a Yocto sysroot) and configure fails if
one is missing: nothing is downloaded. `-DCPM_LOCAL_PACKAGES_ONLY=OFF` lets CPM
fetch what it cannot find, from the release tarball each declaration names and
checks by SHA-256, and `-DCPM_DOWNLOAD_<Name>=ON` fetches that one even when a
local copy exists.

For a change that needs something new in the library, build against a library
checkout instead of the image's release:

```sh
cmake --preset dev -DCPM_util_SOURCE=/work/cxx-cmake-library
```

CI builds the application both ways, against the image's `util` and against the
library's `main`.

## Tests

`ctest --preset dev` runs both tiers; `-L unit` or `-L integration` picks one.

- **Unit** (`test/unit/`, GoogleTest). Everything except `main()` is in the
  `app_core` library, so its modules can be tested directly. Every file in
  `src/` and header in `inc/app/` needs `test/unit/src/<stem>.cpp`, or configure
  fails; a file with nothing to test on its own goes in `APP_TEST_EXEMPT` with
  the reason, as `main` does.
- **Integration** (`test/integration/`, pytest). CTest runs pytest once, passing
  the built binary as `APP_BINARY`, and pytest writes a JUnit file. The tests run
  `app` as a user would and check what it prints. ruff lints the Python, with the
  rules in `pyproject.toml`.

## Profiling

`-DENABLE_TRACY=ON` (or the `tracy` preset) builds the application with Tracy
zones: `APP_ZONE;` at the top of a scope and `APP_FRAME;` at the end of a round
of work, both from `src/profiling.hpp`. With the option off they compile to
nothing and Tracy is not a dependency. Run the binary with the Tracy profiler or
`tracy-capture` listening to record a trace. Unlike the library, which never
contains Tracy, an application is the final binary, so whether it ships with
zones is its own decision.

## CI

`.github/workflows/ci.yml` calls the library template's `_gate.yml` at a release
tag: gcc and clang in Debug and Release running both tiers, ASan and UBSan,
clang-tidy and clang-format, in the container image named by digest. Next to it
are this repository's own jobs: `util` resolving from the image, a build against
the library's `main`, the `tracy` build, and ruff. `post-merge.yml` runs the
library's `_full.yml` on every push to `main`, including native arm64, and
weekly, when `_weekly.yml` opens an issue if it fails. Every commit in a pull
request is linted as a Conventional Commit.

The image and the library tag are the two references to move by hand: take the
new image's version and digests from cxx-cmake-container's release notes, and
the new library tag from its releases, and change every reference in one pull
request.

## Releases

`.github/workflows/release.yml` runs release-please on every push to `main`. It
keeps one rolling pull request that adds the `feat`, `fix`, `perf`, `build` and
`revert` commits since the last release to `CHANGELOG.md` and writes the next
version into `project(VERSION ...)` in `CMakeLists.txt`. Merging it tags
`vX.Y.Z` and creates the GitHub Release. Until 1.0 a breaking change bumps the
minor, like a `feat`.

The workflow needs a fine-grained personal access token in the
`RELEASE_PLEASE_TOKEN` secret: GitHub runs no workflows on a pull request made
with a workflow's own token, so the gate would never run on the release pull
request. Create the token under Settings > Developer settings > Personal access
tokens > Fine-grained tokens, for this repository, with Contents, Pull requests
and Issues read and write, then run `gh secret set RELEASE_PLEASE_TOKEN`.

## Using this template

Create a repository from this template, then make it yours. Rename checklist,
with `myapp` standing for your name:

- [ ] `project("app" ...)` in `CMakeLists.txt`, and its `DESCRIPTION`;
- [ ] the `app` namespace and `inc/app/` (and every `#include <app/...>`);
- [ ] `app_core`, `app_warnings`, `app_unit_tests`, `APP_WARNINGS_AS_ERRORS`,
      `APP_TEST_EXEMPT`, `APP_ENABLE_TRACY`, `APP_ZONE` and `APP_FRAME`;
- [ ] `name` in `.devcontainer/devcontainer.json`;
- [ ] the copyright lines in `LICENSE`, `REUSE.toml` and the SPDX headers.

Replace `src/word_filter.cpp`, `inc/app/word_filter.hpp`, their unit test and
`test/integration/test_app.py` with your own, keep the machinery, reset the
version in `project()` and `.release-please-manifest.json`, delete
`CHANGELOG.md`, and set up the release token.
