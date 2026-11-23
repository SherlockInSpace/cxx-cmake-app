# cxx-cmake-app

Application template for embedded Linux — part of the cxx-cmake template family.

**Placeholder** — the template content is in the issues:
the demo application seeded from the original monorepo, third-party dependencies via CPM
(spdlog), optional Tracy profiling behind `ENABLE_TRACY`, and a pytest-based `integration`
test tier running under CTest.

Family:
[cxx-cmake-library (library template)](https://github.com/SherlockInSpace/cxx-cmake-library) ·
[cxx-cmake-container (build environments)](https://github.com/SherlockInSpace/cxx-cmake-container) ·
[meta-cxx-cmake (Yocto layer)](https://github.com/SherlockInSpace/meta-cxx-cmake)

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
