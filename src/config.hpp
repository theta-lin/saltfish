#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <utility>
#include <variant>
#include <string>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include "io.hpp"

enum class ConfigType
{
	INT,
	DOUBLE,
};

class Config
{
private:
	using config_t = std::variant<int,
		                          double>;
	std::map<std::string, std::pair<ConfigType, config_t> > db;

public:
	void add(const std::string &key, ConfigType type);
	void set(const std::string &key, const std::string &value);
	void loadFromFile(const std::string &fileName);

	void get(const std::string &key, int &target);
	void get(const std::string &key, double &target);
};

#endif // ifndef CONFIG_HPP

