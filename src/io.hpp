#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <cstring>
#include <cctype>
#include <cstddef>
#include <string>
#include <string_view>
#include <sstream>
#include <vector>
#include <list>
#include <array>
#include <stdexcept>

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

using Tokens = std::list<std::string>;
Tokens tokenize(std::string_view data);

template<typename T>
void convertTokens(Tokens::iterator begin, Tokens::iterator end, T &current)
{
	if (begin == end)
		throw std::runtime_error{"convertTokens() failed: wrong number of tokens"};

	std::stringstream stream{*begin};
	stream >> current;
	if (stream.fail())
		throw std::runtime_error{"convertTokens() failed: token type conversion failed"};
}

template<typename T, typename ...Args>
void convertTokens(Tokens::iterator begin, Tokens::iterator end, T &current, Args&... rest)
{
	if (begin == end)
		throw std::runtime_error{"convertTokens() failed: wrong number of tokens"};

	std::stringstream stream{*begin};
	stream >> current;
	if (stream.fail())
		throw std::runtime_error{"convertTokens() failed: token type conversion failed"};

	convertTokens(++begin, end, rest...);
}

#endif // ifndef IO_HPP

