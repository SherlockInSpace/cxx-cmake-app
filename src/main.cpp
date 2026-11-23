// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

// Example application: prints each command-line argument as "new" or "seen",
// using a Bloom filter from the util library. A repeated word is always
// "seen"; a new word may rarely be reported "seen" too, the filter's false
// positive. SPDLOG_LEVEL=debug logs each word as it goes in; a build with
// ENABLE_TRACY shows each word as a Tracy frame.

#include <app/word_filter.hpp>

#include "profiling.hpp"

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <print>
#include <ranges>
#include <span>
#include <string>

int main(int argc, char* argv[])
{
    // Formatting and logging can throw; the error goes to stderr instead of
    // ending the process with std::terminate.
    try {
        spdlog::cfg::load_env_levels();
        auto console = spdlog::stderr_color_mt("app");

        app::WordFilter filter;
        const std::span arguments(argv, static_cast<std::size_t>(argc));
        for (const std::string word : arguments | std::views::drop(1)) {
            APP_ZONE;
            console->debug("Observing word {}", word);
            std::println("{} {}", app::toString(filter.observe(word)), word);
            APP_FRAME;
        }
    } catch (const std::exception& error) {
        std::fputs(error.what(), stderr);
        std::fputs("\n", stderr);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
