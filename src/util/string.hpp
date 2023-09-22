#pragma once

#include <string>

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
int string_fuzzy_match_score(std::string needle_str, std::string haystack_str);
