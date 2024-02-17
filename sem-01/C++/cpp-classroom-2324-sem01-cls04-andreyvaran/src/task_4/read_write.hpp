#include <cstring>

char* read(const char* data, int& out);
char* write(char* data, const int in);
char* read(const char* data, size_t& out);
char* write(char* data, const size_t in);
const char* read(const char* data, char*& out);
char* write(char* data, const char* in);