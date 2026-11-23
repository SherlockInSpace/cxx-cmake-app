// SPDX-FileCopyrightText: 2022-2026 Ryan Sherlock
// SPDX-License-Identifier: MIT

#include <app/word_filter.hpp>

#include <gtest/gtest.h>

#include <string>

namespace {

TEST(wordFilter, firstSightingIsNew)
{
    app::WordFilter filter;
    EXPECT_EQ(filter.observe("alpha"), app::Sighting::kNew);
}

TEST(wordFilter, repeatIsSeen)
{
    app::WordFilter filter;
    ASSERT_EQ(filter.observe("alpha"), app::Sighting::kNew);
    EXPECT_EQ(filter.observe("alpha"), app::Sighting::kSeen);
    EXPECT_EQ(filter.observe("alpha"), app::Sighting::kSeen);
}

// A Bloom filter never forgets, so every word shown once is reported seen
// the second time, whatever came in between.
TEST(wordFilter, everyRepeatIsSeen)
{
    app::WordFilter filter;
    for (int i = 0; i < 100; ++i) {
        static_cast<void>(filter.observe("word-" + std::to_string(i)));
    }
    for (int i = 0; i < 100; ++i) {
        EXPECT_EQ(filter.observe("word-" + std::to_string(i)), app::Sighting::kSeen) << i;
    }
}

TEST(wordFilter, filtersAreIndependent)
{
    app::WordFilter first;
    app::WordFilter second;
    ASSERT_EQ(first.observe("alpha"), app::Sighting::kNew);
    EXPECT_EQ(second.observe("alpha"), app::Sighting::kNew);
}

TEST(wordFilter, sightingNames)
{
    EXPECT_EQ(app::toString(app::Sighting::kNew), "new");
    EXPECT_EQ(app::toString(app::Sighting::kSeen), "seen");
}

} // namespace
