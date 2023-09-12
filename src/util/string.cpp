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

/**
 * This is a modified Wagner–Fischer string edit distance algorithm
 * which scores any series of insertions from a -> b as 1, instead
 * of the length of the series of insertions. Effectively, this makes
 * it suited to finding close substring matches, like
 * "sub" in "can you find the sub string".
 */
int string_sub_distance_insensitive(std::string a_str, std::string b_str) {
    const auto a_length = a_str.size();
    const auto b_length = b_str.size();
    const auto matrix_length = (
        (1 + a_length) * (1 + b_length)
    );
    auto score_matrix = std::vector<int>(matrix_length);
    auto edit_matrix = std::vector<int>(matrix_length);
    const int row_length = (int) (a_length + 1);
    for(int i = 0; i < a_length; i++) {
        score_matrix[i] = i;
        edit_matrix[i] = 0;
    }
    for(int j = 1; j < b_length; j++) {
        score_matrix[j * row_length] = j;
        edit_matrix[j * row_length] = 0;
    }
    auto a_iter = a_str.begin();
    for(int i = 1; i < a_length; i++) {
        const char a_char = std::toupper(*a_iter);
        auto b_iter = b_str.begin();
        for(int j = 1; j < b_length; j++) {
            const char b_char = std::toupper(*b_iter);
            const int cell = i + j * row_length;
            const int del_score = score_matrix[cell - 1];
            const int ins_score = (
                score_matrix[cell - row_length] +
                (edit_matrix[cell - row_length] == 1 ? 0 : 1)
            );
            const int sub_score = (
                score_matrix[cell - row_length - 1] +
                (a_char == b_char ? 0 : 1)
            );
            const int min_score_partial = (
                del_score < ins_score ?
                del_score : ins_score
            );
            const int min_score = (
                sub_score < min_score_partial ?
                sub_score : min_score_partial
            );
            if(min_score == ins_score) {
                edit_matrix[cell] = 1;
            }
            else {
                edit_matrix[cell] = 0;
            }
            score_matrix[cell] = min_score;
            ++b_iter;
        }
        ++a_iter;
    }
    return score_matrix[matrix_length - 1];
}
