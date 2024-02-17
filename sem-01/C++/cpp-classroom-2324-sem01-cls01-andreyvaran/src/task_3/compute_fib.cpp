#include "compute_fib.hpp"
#include <iostream>
#include <fstream>

int64_t compute_fib() {
    std::ifstream input("input.txt");

    if (!input.is_open()) {
        std::cerr << "Error while read input.txt" << std::endl;
        return -1;
    }

    int N;
    input >> N;

    input.close();

    int64_t fib[N + 1];
    fib[0] = 0;
    fib[1] = 1;

    for (int i = 2; i <= N; ++i) {
        fib[i] = fib[i - 1] + fib[i - 2];
    }

    return fib[N];
}
