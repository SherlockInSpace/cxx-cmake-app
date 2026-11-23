// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

/// @file
/// @brief Reports each word as new or already seen, using util's Bloom filter.

#pragma once

#include <util/bloom.hpp>

#include <cstdint>
#include <string>
#include <string_view>

namespace app {

/// Whether a word had been seen before. A Bloom filter can mistake a new word
/// for a seen one (a false positive) but never the other way round, so
/// kNew is always right and kSeen means "probably seen".
enum class Sighting : std::uint8_t
{
    kNew, ///< The word had not been observed before.
    kSeen ///< The word had probably been observed before.
};

/// The word the application prints for @p sighting: "new" or "seen".
[[nodiscard]] std::string_view toString(Sighting sighting);

/// Remembers the words it is shown and reports each one as new or seen.
class WordFilter
{
public:
    /// Reports whether @p word was seen before, then remembers it.
    [[nodiscard]] Sighting observe(const std::string& word);

private:
    util::Bloom filter_;
};

} // namespace app
