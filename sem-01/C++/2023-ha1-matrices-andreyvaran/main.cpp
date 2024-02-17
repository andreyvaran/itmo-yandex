#include "matrices.hpp"
#include <iostream>
#include <cstring>

int main(int argc, char *argv[]) {

    Matrix result(argv[1]);
    for (int i = 2; i < argc; i += 2) {

        if (i + 1 < argc) {
            if (strcmp(argv[i], "--add") == 0) {

                if (i + 1 < argc) {
                    Matrix otherMatrix(argv[i + 1]);
                    result.add(otherMatrix);
                } else {
                    std::cerr << "Missing operand for --add." << std::endl;
                    return 1;
                }
            } else if (strcmp(argv[i], "--mult") == 0) {
                if (i + 1 < argc) {
                    Matrix otherMatrix(argv[i + 1]);
                    result.multiply(otherMatrix);
                } else {
                    std::cerr << "Missing operand for --mult." << std::endl;
                    return 1;
                }
            } else {
                std::cerr << "Invalid operation: " << argv[i] << std::endl;
                return 1;
            }
        } else {
            std::cerr << "Missing operand for " << argv[i] << std::endl;
            return 1;
        }
    }
    result.print();
    return 0;
}
