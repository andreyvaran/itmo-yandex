#include "composition.hpp"
#include <cstdarg>
#include <iostream>


double Compose(size_t n, ...) {
    va_list args;
    va_start(args, n);

    double result = va_arg(args, double);
//    std::cout << "result is "<<result << std::endl;
    for (size_t i = 0; i < n; ++i) {
        FuncT func = va_arg(args, FuncT);
//        std::cout << "func(result) is "<<func(result) << std::endl;
        result = func(result);
    }
    va_end(args);
    return result;

}