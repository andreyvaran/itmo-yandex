#include "concat.hpp"
#include <cstring>
#include <cstddef>

char const* concat(
        char const a[],
        size_t a_size,
        char const b[],
        size_t b_size,
        size_t& concat_size
) {
    // Вычисляем размер объединенной строки
    concat_size = a_size + b_size;

    // Выделяем память под объединенную строку
    char* result = new char[concat_size + 1]; // +1 для завершающего нулевого символа

    // Копируем данные из первой строки (a) в результат
    std::memcpy(result, a, a_size);

    // Копируем данные из второй строки (b) в результат, начиная с конца первой строки
    std::memcpy(result + a_size, b, b_size);

    // Добавляем завершающий нулевой символ
    result[concat_size] = '\0';

    return result;
}