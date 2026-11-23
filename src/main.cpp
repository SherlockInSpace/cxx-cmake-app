// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

// Example application: puts each command-line argument into a Bloom filter
// from the util library and logs it. SPDLOG_LEVEL=debug shows the log lines.

#include <util/bloom.hpp>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <span>

int main(int argc, char* argv[])
{
    spdlog::cfg::load_env_levels();
    auto console = spdlog::stdout_color_mt("app");

    util::Bloom bloomFilter;
    const std::span arguments(argv, static_cast<std::size_t>(argc));
    for (std::size_t idx = 1; idx < arguments.size(); ++idx) {
        bloomFilter.insert(arguments[idx]);
        console->debug("Inserting word {} at index {}", arguments[idx], idx);
    }
    return 0;
}
