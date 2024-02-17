#include <iostream>
#include "combine.hpp"


int combine(const double *arr, int length, double (*mult)(double, double), double (*plus)(double, double)) {
    int result = 0;

    if (length==0){
        return result;
    }
    if (length == 1) {
        return arr[0];
    }
    if (length % 2 == 0) {
        result = mult(arr[0], arr[1]);
        for (int i = 2; i < length; i += 2) {
            result = plus(result, mult(arr[i], arr[i + 1]));
        }
    } else {
        result = mult(arr[0], arr[1]);
        for (int i = 2; i < length - 1; i += 2) {
//            std::cout << result << " " << plus(result, arr[length - 1]) << std::endl;
            result = plus(result, mult(arr[i], arr[i + 1]));
        }
//        std::cout << result << " " << plus(result, arr[length - 1]) << std::endl;
        result = plus(result, arr[length - 1]);
    }

    return static_cast<int>(result);
}

