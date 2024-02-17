#include "ladder_combination.hpp"

//#include <iostream>
//#include <cstdint>

const int MAX_N = 21;

uint64_t countLadderCombinations(uint8_t steps, uint64_t memo[]) {
    if (steps <= 0) {
        return 0;
    } else if (steps == 1){
        return 1;
    }

    if (memo[steps] != 0) {
        return memo[steps];
    }

    memo[steps] =
            countLadderCombinations(steps - 1, memo) + countLadderCombinations(steps - 2, memo);

    return memo[steps];
}

uint64_t ladderCombinations(uint8_t steps) {
    uint64_t memo[MAX_N] = {0};
    return countLadderCombinations(steps, memo);
}
