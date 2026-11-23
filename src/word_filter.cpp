// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

#include <app/word_filter.hpp>

#include <string>
#include <string_view>

namespace app {

std::string_view toString(Sighting sighting)
{
    switch (sighting) {
    case Sighting::kNew:
        return "new";
    case Sighting::kSeen:
        return "seen";
    }
    return "unknown";
}

Sighting WordFilter::observe(const std::string& word)
{
    const Sighting sighting = filter_.contains(word) ? Sighting::kSeen : Sighting::kNew;
    filter_.insert(word);
    return sighting;
}

} // namespace app
