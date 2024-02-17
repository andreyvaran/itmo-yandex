#include "find_nearest_same.hpp"

ptrdiff_t findNearestSame(const char *s, const char *c) {
    if (*c == '\0') {
        return 0;
    }

    const char *left = c - 1;
    const char *right = c + 1;

    while (true) {

        if (*left == *c) {
            return left - c;
        }
        if (*right == *c) {
            return right - c;
        }
        if (*left == '\0' && *right == '\0') {
            return 0;
        }

        if (*left != '\0') {
            left--;
        }
        if (*right != '\0') {
            right++;
        }
    }
}
