#include <cstring>
#include "read_write.hpp"


char *read(const char *data, int &out) {
    std::memcpy(&out, data, sizeof(int));
    return const_cast<char *>(data + sizeof(int));
}

char *write(char *data, const int in) {
    std::memcpy(data, &in, sizeof(int));
    return data + sizeof(int);
}

char *read(const char *data, size_t &out) {
    std::memcpy(&out, data, sizeof(size_t));
    return const_cast<char *>(data + sizeof(size_t));
}

char *write(char *data, const size_t in) {
    std::memcpy(data, &in, sizeof(size_t));
    return data + sizeof(size_t);
}

const char* read(const char* data, char*& out) {
    const size_t len = strlen(data);
    out = new char[len + 1];
    strcpy(out, data);
    return data + len + 1;
}
char *write(char *data, const char *in) {
    const size_t len = strlen(in);
    strcpy(data, in);
    return data + len + 1;
}
