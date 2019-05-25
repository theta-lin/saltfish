#ifndef IO_HPP
#define IO_HPP

#include <cstdint>
#include <cstring>
#include <fstream>
#include <string>
#include <vector>
//#include "world.hpp"

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

//class DataBuffer
//{
//private:
//    std::vector<char> buffer;

//public:
//    DataBuffer()
//    {
//    }

//    DataBuffer(const std::string &path)
//    {
//        load(path);
//    }

//    DataBuffer(const World &world)
//    {
//        load(World);
//    }

//    bool load(const std::string &path);
//    bool load(const World &world);
//    bool write(const std::string &path);
//    bool write(const World &world);
//};

#endif // ifndef IO_HPP

