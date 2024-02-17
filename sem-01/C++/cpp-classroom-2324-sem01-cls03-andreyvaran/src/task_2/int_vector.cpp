#include <iostream>
#include "int_vector.hpp"


void pushBack(IntVector& v, int value) {
    if (v.size == v.capacity_) {
        size_t newCapacity = (v.capacity_ == 0) ? 1 : (v.capacity_ * 2);
        int* newData = new int[newCapacity];

        for (size_t i = 0; i < v.size; ++i) {
            newData[i] = v.data[i];
        }
        delete[] v.data;

        v.data = newData;
        v.capacity_ = newCapacity;
    }

    v.data[v.size++] = value;
}

void popBack(IntVector& v) {
    if (v.size > 0) {
        --v.size;
    }
}

void deallocate(IntVector& v) {
    if (v.data != nullptr) {
        delete[] v.data;
        v.data = nullptr;
        v.size = 0;
        v.capacity_ = 0;
    }
}