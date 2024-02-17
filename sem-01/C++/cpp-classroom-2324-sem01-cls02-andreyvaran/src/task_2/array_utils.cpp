#include "array_utils.hpp"

int sum(const int *arr, size_t size) {
    int result = 0;
    for (size_t i = 0; i < size; ++i) {
        result += arr[i];
    }
    return result;
}

int countNumbers(const int *arr, size_t size) {
    bool seen[10] = {false};
    int count = 0;

    for (size_t i = 0; i < size; ++i) {
        if (arr[i] >= 0 && arr[i] <= 9 && !seen[arr[i]]) {
            seen[arr[i]] = true;
            count++;
        }
    }

    return count;
}


void popZeros(const_int_pointer_t *a, size_t size) {
    size_t writeIndex = 0;

    for (size_t i = 0; i < size; ++i) {
        if (*a[i] != 0) {
            a[writeIndex++] = a[i];
        }
    }

    while (writeIndex < size) {
        a[writeIndex++] = nullptr;
    }
}
