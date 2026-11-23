# Third-party notices

This repository vendors (copies into the tree) the components listed below.
Each keeps its upstream licence and copyright notice and its upstream style:
vendored files are left out of formatting and naming checks. Update this file
whenever a vendored file is added, refreshed from upstream, or removed.

The rest of the repository is licensed under the terms in [LICENSE](LICENSE).

## CPM.cmake

| | |
|---|---|
| File(s) | `cmake/CPM.cmake` |
| Origin | <https://github.com/cpm-cmake/CPM.cmake> |
| Licence | MIT (notice reproduced at the top of `cmake/CPM.cmake`) |
| Copyright | Copyright (c) 2019-2023 Lars Melchior and contributors (as the vendored file states) |
| Version | release `v0.43.1`, the `CPM.cmake` release asset |
| SHA-256 | `1c40fc102ce9625d7de7eb14f541cab30cc3138dca627f0b0ec40293ce6c2934` |

`cmake/CPM.cmake` is the release asset byte for byte, the same file the library
template vendors. Vendoring it means configure never downloads CPM itself. To
update, replace the file with a newer release's `CPM.cmake` asset, check its
SHA-256 against the release, and update the version and hash here.
