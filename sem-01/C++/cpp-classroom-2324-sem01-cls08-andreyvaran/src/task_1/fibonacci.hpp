#include <cstddef>
#include <cstdint>
#pragma once
namespace cls08 {
    template<std::size_t N>
    struct FibonacciNumber {
        static const uint64_t value = FibonacciNumber<N - 1>::value + FibonacciNumber<N - 2>::value;
    };

    template<>
    struct FibonacciNumber<0> {
        static const uint64_t value = 0;
    };

    template<>
    struct FibonacciNumber<1> {
        static const uint64_t value = 1;
    };
}
