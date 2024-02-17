#include "matrices.hpp"
#include <fstream>
#include <iostream>

Matrix::Matrix(int rows, int cols) : rows(rows), cols(cols) {
    data = new double[rows * cols];
}

Matrix::~Matrix() {
    delete[] data;
}

Matrix::Matrix(const char *filename) : rows(0), cols(0), data(nullptr) {
    initializeFromFile(filename);
}

void Matrix::initializeFromFile(const char *filename) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file: " << filename << std::endl;
        exit(1);
    }

    file >> rows >> cols;

    data = new double[rows * cols];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            file >> data[i * cols + j];
        }
    }

    file.close();
}


void Matrix::add(const Matrix &other) {

    if ((cols != other.cols) ||  (rows != other.rows)) {
        std::cerr << "Matrix dimensions are not suitable for add." << std::endl;
        exit(1);
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data[i * cols + j] += other.data[i * cols + j];
        }
    }
}

void Matrix::multiply(const Matrix &other) {
    if (cols != other.rows) {
        std::cerr << "Matrix dimensions are not suitable for multiplication." << std::endl;
        exit(1);
    }

    double *result = new double[rows * other.cols];

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < other.cols; j++) {
            result[i * other.cols + j] = 0;
            for (int k = 0; k < cols; k++) {
                result[i * other.cols + j] += data[i * cols + k] * other.data[k * other.cols + j];
            }
        }
    }

    delete[] data;
    data = result;
    cols = other.cols;
}

void Matrix::print() const {
    std::cout << rows << ' ' << cols << std::endl;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout << data[i * cols + j] << ' ';
        }
        std::cout << std::endl;
    }
}

int Matrix::getRows() const {
    return rows;
}

int Matrix::getCols() const {
    return cols;
}
