#pragma once

#include <string>

// Type returned by string_fuzzy_match.
struct StringFuzzyMatchResult {
    // String match score. Higher is better.
    int score;
    // Total number of characters in the needle string that
    // matched a corresponding character in the haystack string.
    int matched;
};

/**
 * Case-insensitive comparison of start of string.
 * 
 * Returns true when the beginning of a_str is equivalent to the
 * entirety of b_str.
 * 
 * Only ASCII characters are compared case-insensitively.
 * Does not perform unicode normalization.
 */
bool string_starts_with_insensitive(std::string a_str, std::string b_str);

/**
 * TODO: Document this
 */
StringFuzzyMatchResult string_fuzzy_match(
    std::string needle_str, std::string haystack_str
);
