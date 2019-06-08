#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <cstring>
#include <cctype>
#include <string>
#include <string_view>
#include <list>
#include <array>

char* serial(uint32_t value, char* buffer);
char* serial(int32_t value, char* buffer);
char* serial(uint64_t value, char* buffer);
char* serial(int64_t value, char* buffer);
char* serial(double value, char* buffer);

char* deserial(uint32_t &value, char *buffer);
char* deserial(int32_t &value, char *buffer);
char* deserial(uint64_t &value, char *buffer);
char* deserial(int64_t &value, char *buffer);
char* deserial(double &value, char *buffer);

std::list<std::string> tokenize(std::string_view data);

#endif // ifndef IO_HPP

