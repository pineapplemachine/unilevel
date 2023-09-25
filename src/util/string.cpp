#include "string.hpp"

#include <string>
#include <vector>

bool string_starts_with_insensitive(std::string a_str, std::string b_str) {
    const auto a_length = a_str.size();
    const auto b_length = b_str.size();
    if(a_length < b_length) {
        return false;
    }
    for(int i = 0; i < b_length; ++i) {
        if(std::toupper(a_str[i]) != std::toupper(b_str[i])) {
            return false;
        }
    }
    return true;
}

struct SubFuzzyMatchState {
    // Length of a current run of matching characters
    int run_current = 0;
    // Length of first run of matching characters encountered
    int run_first = 0;
    // Length of run of matching characters starting at the
    // very first character in the string, if there was a run
    int run_initial = 0;
    // Longest run of matching characters yet encountered
    int run_longest = 0;
    // Total number of matched characters
    int match_count = 0;
    // Total number of matched characters where the previous
    // character was also a match
    int match_count_run = 0;
    // Number of matched alphanumeric characters that followed
    // a non-alphanumeric character in the haystack string
    int match_boundary_count = 0;
    // Score assigned to this matcher state
    int score = 0;
};

// TODO: move into own file
bool ascii_is_word_char(const char ch) {
    return ch == '_' || (
        (ch >= '0' && ch <= '9') ||
        (ch >= 'a' && ch <= 'z') ||
        (ch >= 'A' && ch <= 'Z')
    );
}

// TODO: maybe?
// https://github.com/doctest/doctest
// UNI_TEST_CASE("ascii_is_word_char") {
//     assert(ascii_is_word_char('_'));
//     assert(ascii_is_word_char('0'));
//     assert(ascii_is_word_char('1'));
//     assert(ascii_is_word_char('8'));
//     assert(ascii_is_word_char('9'));
//     assert(ascii_is_word_char('a'));
//     assert(ascii_is_word_char('b'));
//     assert(ascii_is_word_char('x'));
//     assert(ascii_is_word_char('z'));
//     assert(ascii_is_word_char('A'));
//     assert(ascii_is_word_char('B'));
//     assert(ascii_is_word_char('X'));
//     assert(ascii_is_word_char('Z'));
//     assert(!ascii_is_word_char('?'));
//     assert(!ascii_is_word_char('.'));
//     assert(!ascii_is_word_char('!'));
//     assert(!ascii_is_word_char('@'));
//     assert(!ascii_is_word_char('='));
//     assert(!ascii_is_word_char('-'));
//     assert(!ascii_is_word_char('/'));
// }

inline int state_get_score(SubFuzzyMatchState* state) {
    return 1 + (
        state->run_first +
        state->run_initial +
        (state->run_longest << 1) +
        (state->match_count << 1) +
        (state->match_count_run << 1) +
        state->match_boundary_count
    );
}

// TODO: would be good to factor in index of first matched char in score
StringFuzzyMatchResult string_fuzzy_match(
    std::string needle_str, std::string haystack_str
) {
    const int needle_length = (int) needle_str.size();
    const int haystack_length = (int) haystack_str.size();
    const int row_length = 1 + needle_length;
    const int col_length = 1 + haystack_length;
    const int state_matrix_length = row_length * col_length;
    // TODO: std::vector is not needed here. optimize this
    auto state_matrix = std::vector<SubFuzzyMatchState>(state_matrix_length);
    // Initialization step not needed due to std::vector
    // for(int i = 0; i < row_length; ++i) {
    //     state_matrix[i] = SubFuzzyMatchState{};
    // }
    // for(int j = 1; j < col_length; ++j) {
    //     state_matrix[j * row_length] = SubFuzzyMatchState{};
    // }
    for(int i = 1; i < row_length; ++i) {
        const char needle_char = std::toupper(needle_str[i - 1]);
        const auto needle_char_is_word_char = ascii_is_word_char(needle_char);
        for(int j = 1; j < col_length; ++j) {
            const char haystack_char = std::toupper(haystack_str[j - 1]);
            const int cell_i = i + j * row_length;
            const auto state_prev_j = &state_matrix[cell_i - row_length];
            const int score_prev_j = state_get_score(state_prev_j);
            if(needle_char == haystack_char) {
                const bool haystack_char_word_boundary = (
                    j <= 1 ? true : !ascii_is_word_char(haystack_str[j - 2])
                );
                const auto state_prev_ij = &state_matrix[cell_i - row_length - 1];
                const int run_current = 1 + state_prev_ij->run_current;
                const int match_count = 1 + state_prev_ij->match_count;
                auto state_match = SubFuzzyMatchState{
                    .run_current = run_current,
                    .run_first = (
                        match_count == run_current ?
                        run_current : state_prev_ij->run_first
                    ),
                    .run_initial = (
                        run_current >= j ?
                        run_current : state_prev_ij->run_initial
                    ),
                    .run_longest = (
                        run_current > state_prev_ij->run_longest ?
                        run_current : state_prev_ij->run_longest
                    ),
                    .match_count = match_count,
                    .match_count_run = (
                        state_prev_ij->match_count_run +
                        (state_prev_ij->run_current > 0 ? 1 : 0)
                    ),
                    .match_boundary_count = (
                        state_prev_ij->match_boundary_count + ((
                            state_prev_ij->run_current == 0 &&
                            needle_char_is_word_char &&
                            haystack_char_word_boundary
                        ) ? 1 : 0)
                    )
                };
                state_match.score = state_get_score(&state_match);
                if(state_match.score >= state_prev_j->score) {
                    state_matrix[cell_i] = state_match;
                }
                else {
                    state_matrix[cell_i] = *state_prev_j;
                    state_matrix[cell_i].run_current = 0;
                }
            }
            else {
                const auto state_skip = &state_matrix[cell_i - 1];
                if(state_skip->score >= state_prev_j->score) {
                    state_matrix[cell_i] = *state_skip;
                }
                else {
                    state_matrix[cell_i] = *state_prev_j;
                    state_matrix[cell_i].run_current = 0;
                }
            }
        }
    }
    // const int score_final = (
    //     (needle_str.size() << 3) -
    //     state_matrix[state_matrix_length - 1].score
    // );
    // Potentially useful for debugging purposes
    // const auto state_final = &state_matrix[state_matrix_length - 1];
    // spdlog::trace("string_fuzzy_match_score {} {} {}", score_final, needle_str, haystack_str);
    // spdlog::trace("  run_current: {}", state_final->run_current);
    // spdlog::trace("  run_longest: {}", state_final->run_longest);
    // spdlog::trace("  run_first: {}", state_final->run_first);
    // spdlog::trace("  run_initial: {}", state_final->run_initial);
    // spdlog::trace("  match_count: {}", state_final->match_count);
    // spdlog::trace("  match_count_run: {}", state_final->match_count_run);
    // spdlog::trace("  match_boundary_count: {}", state_final->match_boundary_count);
    // spdlog::trace("  score: {}", state_final->score);
    // return score_final;
    return StringFuzzyMatchResult{
        .score = state_matrix[state_matrix_length - 1].score,
        .matched = state_matrix[state_matrix_length - 1].match_count
    };
}
