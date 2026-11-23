// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

// Example application: prints each command-line argument as "new" or "seen",
// using a Bloom filter from the util library. A repeated word is always
// "seen"; a new word may rarely be reported "seen" too, the filter's false
// positive. SPDLOG_LEVEL=debug logs each word as it goes in.

#include <app/word_filter.hpp>

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <print>
#include <span>
#include <string>

int main(int argc, char* argv[])
{
    spdlog::cfg::load_env_levels();
    auto console = spdlog::stderr_color_mt("app");

    app::WordFilter filter;
    const std::span arguments(argv, static_cast<std::size_t>(argc));
    for (std::size_t idx = 1; idx < arguments.size(); ++idx) {
        const std::string word = arguments[idx];
        console->debug("Observing word {} at index {}", word, idx);
        std::println("{} {}", app::toString(filter.observe(word)), word);
    }
    return 0;
}
