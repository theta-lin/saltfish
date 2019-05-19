#include "io.hpp"

char* serial(uint32_t value, char *buffer)
{
	buffer[0] = static_cast<char>(value >> 24);
	buffer[1] = static_cast<char>(value >> 16);
	buffer[2] = static_cast<char>(value >> 8);
	buffer[3] = static_cast<char>(value);
	return buffer + 4;
}

char* serial(int32_t value, char *buffer)
{
	buffer[0] = static_cast<char>(value >> 24);
	buffer[1] = static_cast<char>(value >> 16);
	buffer[2] = static_cast<char>(value >> 8);
	buffer[3] = static_cast<char>(value);
	return buffer + 4;
}

char* serial(uint64_t value, char *buffer)
{
	buffer[0] = static_cast<char>(value >> 56);
	buffer[1] = static_cast<char>(value >> 48);
	buffer[2] = static_cast<char>(value >> 40);
	buffer[3] = static_cast<char>(value >> 32);
	buffer[4] = static_cast<char>(value >> 24);
	buffer[5] = static_cast<char>(value >> 16);
	buffer[6] = static_cast<char>(value >> 8);
	buffer[7] = static_cast<char>(value);
	return buffer + 8;
}

char* serial(int64_t value, char *buffer)
{
	buffer[0] = static_cast<char>(value >> 56);
	buffer[1] = static_cast<char>(value >> 48);
	buffer[2] = static_cast<char>(value >> 40);
	buffer[3] = static_cast<char>(value >> 32);
	buffer[4] = static_cast<char>(value >> 24);
	buffer[5] = static_cast<char>(value >> 16);
	buffer[6] = static_cast<char>(value >> 8);
	buffer[7] = static_cast<char>(value);
	return buffer + 8;
}

char* serial(double value, char *buffer)
{
	uint64_t ivalue;
	memcpy(&ivalue, &value, sizeof(value));
	return serial(ivalue, buffer);
}

char* deserial(uint32_t &value, char *buffer)
{
	value =   (static_cast<uint32_t>(buffer[0]) << 24)
			+ (static_cast<uint32_t>(buffer[1]) << 16)
			+ (static_cast<uint32_t>(buffer[2]) << 8)
			+ (static_cast<uint32_t>(buffer[3]));
	return buffer + 4;
}

char* deserial(int32_t &value, char *buffer)
{
	value =   (static_cast<int32_t>(buffer[0]) << 24)
			+ (static_cast<int32_t>(buffer[1]) << 16)
			+ (static_cast<int32_t>(buffer[2]) << 8)
			+ (static_cast<int32_t>(buffer[3]));
	return buffer + 4;
}

char* deserial(uint64_t &value, char *buffer)
{
	value =   (static_cast<uint64_t>(buffer[0]) << 56)
			+ (static_cast<uint64_t>(buffer[1]) << 48)
			+ (static_cast<uint64_t>(buffer[2]) << 40)
			+ (static_cast<uint64_t>(buffer[3]) << 32)
			+ (static_cast<uint64_t>(buffer[4]) << 24)
			+ (static_cast<uint64_t>(buffer[5]) << 16)
			+ (static_cast<uint64_t>(buffer[6]) << 8)
			+ (static_cast<uint64_t>(buffer[7]));

	return buffer + 8;
}

char* deserial(int64_t &value, char *buffer)
{
	value =   (static_cast<int64_t>(buffer[0]) << 56)
			+ (static_cast<int64_t>(buffer[1]) << 48)
			+ (static_cast<int64_t>(buffer[2]) << 40)
			+ (static_cast<int64_t>(buffer[3]) << 32)
			+ (static_cast<int64_t>(buffer[4]) << 24)
			+ (static_cast<int64_t>(buffer[5]) << 16)
			+ (static_cast<int64_t>(buffer[6]) << 8)
			+ (static_cast<int64_t>(buffer[7]));

	return buffer + 8;
}

char* deserial(double &value, char *buffer)
{
	uint64_t ivalue;
	deserial(ivalue, buffer);
	memcpy(&value, &ivalue, sizeof(value));
	return buffer + 8;
}
