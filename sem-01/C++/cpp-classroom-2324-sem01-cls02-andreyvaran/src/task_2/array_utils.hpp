#include <cstddef>

using const_int_pointer_t = const int*;


int sum(const_int_pointer_t a, size_t s);
int countNumbers(const_int_pointer_t a, size_t s);
void popZeros(const_int_pointer_t *a, size_t s);
