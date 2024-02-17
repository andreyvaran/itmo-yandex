#include <cstddef>

using FuncT = double(*)(double);

// Композиция всех переданных функций
double Compose(size_t n, ...);
