#ifndef MATRICES_HPP
#define MATRICES_HPP

class Matrix {
public:
    Matrix(int rows, int cols);

    ~Matrix();

    Matrix(const char *filename);

    void add(const Matrix &other);

    void multiply(const Matrix &other);

    void print() const;

    int getRows() const;

    int getCols() const;

private:
    void initializeFromFile(const char *filename);

    int rows;
    int cols;
    double *data;
};

#endif
