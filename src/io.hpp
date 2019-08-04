#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <cstring>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>
#include <list>
#include <array>

void serial(uint16_t value, std::vector<std::byte> &buffer);
void serial(int16_t value, std::vector<std::byte> &buffer);
void serial(uint32_t value, std::vector<std::byte> &buffer);
void serial(int32_t value, std::vector<std::byte> &buffer);
void serial(uint64_t value, std::vector<std::byte> &buffer);
void serial(int64_t value, std::vector<std::byte> &buffer);
void serial(double value, std::vector<std::byte> &buffer);

void deserial(uint16_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(int16_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(uint32_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(int32_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(uint64_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(int64_t &value, std::vector<std::byte> &buffer, std::size_t &index);
void deserial(double &value, std::vector<std::byte> &buffer, std::size_t &index);

std::list<std::string> tokenize(std::string_view data);

#endif // ifndef IO_HPP

