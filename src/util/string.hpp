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
 * Compute a case-insensitive string edit distance.
 * 
 * This string distance function scores runs of insertion
 * edits as just 1, so "sub" scores the same against
 * "substitute" or "substring".
 * 
 * The first argument should normally be a search term and
 * the second argument should normally be a string being
 * matched against.
 * 
 * Only ASCII characters are compared case-insensitively.
 * Does not perform unicode normalization.
 */
int string_sub_distance_insensitive(std::string a_str, std::string b_str);
