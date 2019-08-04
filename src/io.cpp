#include "io.hpp"

void serial(uint16_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(int16_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(uint32_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 24));
	buffer.push_back(static_cast<std::byte>(value >> 16));
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(int32_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 24));
	buffer.push_back(static_cast<std::byte>(value >> 16));
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(uint64_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 56));
	buffer.push_back(static_cast<std::byte>(value >> 48));
	buffer.push_back(static_cast<std::byte>(value >> 40));
	buffer.push_back(static_cast<std::byte>(value >> 32));
	buffer.push_back(static_cast<std::byte>(value >> 24));
	buffer.push_back(static_cast<std::byte>(value >> 16));
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(int64_t value, std::vector<std::byte> &buffer)
{
	buffer.push_back(static_cast<std::byte>(value >> 56));
	buffer.push_back(static_cast<std::byte>(value >> 48));
	buffer.push_back(static_cast<std::byte>(value >> 40));
	buffer.push_back(static_cast<std::byte>(value >> 32));
	buffer.push_back(static_cast<std::byte>(value >> 24));
	buffer.push_back(static_cast<std::byte>(value >> 16));
	buffer.push_back(static_cast<std::byte>(value >> 8));
	buffer.push_back(static_cast<std::byte>(value));
}

void serial(double value, std::vector<std::byte> &buffer)
{
	uint64_t ivalue;
	memcpy(&ivalue, &value, sizeof(value));
	serial(ivalue, buffer);
}

void deserial(uint16_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value  = static_cast<uint16_t>(buffer.at(index++)) << 8;
	value += static_cast<uint16_t>(buffer.at(index++));
}

void deserial(int16_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value = static_cast<int16_t>(buffer.at(index++)) << 8;
	value += static_cast<int16_t>(buffer.at(index++));
}

void deserial(uint32_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value  = static_cast<uint32_t>(buffer.at(index++)) << 24;
	value += static_cast<uint32_t>(buffer.at(index++)) << 16;
	value += static_cast<uint32_t>(buffer.at(index++)) << 8;
	value += static_cast<uint32_t>(buffer.at(index++));
}

void deserial(int32_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value  = static_cast<int32_t>(buffer.at(index++)) << 24;
	value += static_cast<int32_t>(buffer.at(index++)) << 16;
	value += static_cast<int32_t>(buffer.at(index++)) << 8;
	value += static_cast<int32_t>(buffer.at(index++));
}

void deserial(uint64_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value  = static_cast<uint64_t>(buffer.at(index++)) << 56;
	value += static_cast<uint64_t>(buffer.at(index++)) << 48;
	value += static_cast<uint64_t>(buffer.at(index++)) << 40;
	value += static_cast<uint64_t>(buffer.at(index++)) << 32;
	value += static_cast<uint64_t>(buffer.at(index++)) << 24;
	value += static_cast<uint64_t>(buffer.at(index++)) << 16;
	value += static_cast<uint64_t>(buffer.at(index++)) << 8;
	value += static_cast<uint64_t>(buffer.at(index++));
}

void deserial(int64_t &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	value  = static_cast<int64_t>(buffer.at(index++)) << 56;
	value += static_cast<int64_t>(buffer.at(index++)) << 48;
	value += static_cast<int64_t>(buffer.at(index++)) << 40;
	value += static_cast<int64_t>(buffer.at(index++)) << 32;
	value += static_cast<int64_t>(buffer.at(index++)) << 24;
	value += static_cast<int64_t>(buffer.at(index++)) << 16;
	value += static_cast<int64_t>(buffer.at(index++)) << 8;
	value += static_cast<int64_t>(buffer.at(index++));
}

void deserial(double &value, std::vector<std::byte> &buffer, std::size_t &index)
{
	uint64_t ivalue;
	deserial(ivalue, buffer, index);
	memcpy(&value, &ivalue, sizeof(value));
}

std::list<std::string> tokenize(std::string_view data)
{
	static const std::array<char, 1> operators{'='};

	std::list<std::string> out;
	bool quoted{false};
	bool escape{false};
	bool next{true};
	for (char current : data)
	{
		if (quoted)
		{
			if (!escape && current == '\"')
			{
				quoted = false;
				escape = false;
				next = true;
			}
			else if (!escape && current == '\\')
			{
				escape = true;
			}
			else if (next)
			{
				out.push_back({current});
				next = false;
			}
			else
			{
				out.back().push_back(current);
				escape = false;
			}
		}
		else
		{
			if (std::isspace(current))
			{
				next = true;
			}
			else
			{
				bool isOperator{false};
				for (char element : operators)
				{
					if (current == element)
					{
						isOperator = true;
						break;
					}
				}

				if (isOperator)
				{
					out.push_back({current});
					next = true;
				}
				else
				{
					if (current == '\"')
					{
						quoted = true;
						next = true;
					}
					else if (next)
					{
						out.push_back({current});
						next = false;
					}
					else
					{
						out.back().push_back(current);
					}
				}
			}
		}
	}

	return out;
}

