#include "singleton.hpp"


int inc() {
    static int count = 0;
    return count++;
}

