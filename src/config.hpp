#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "log.hpp"
#include "io.hpp"

/*
 * A simple database to load configuration from files,
 * store them as strings, and convert them to appropriate types.
 */
class Config
{
private:
	Log &logger;
	std::map<std::string, std::string> db;

public:
	Config (Log &logger) : logger{logger}
	{
	}

	void set(const std::string &key, const std::string &value)
	{
		db[key] = value;
	}

	bool loadFromFile(const std::string &fileName)
	{
		std::ifstream inFile{fileName};
		if (!inFile)
		{
			WRITE_LOG(logger, Log::error,
			          "Config::loadFromFile() failed: unable to open \""
			       << fileName << "\"" << std::endl);
			return false;
		}

		int lineCount{1};
		while (inFile)
		{
			std::string line;
			std::getline(inFile, line);

			auto tokens{tokenize(line)};
			if (tokens.size() == 0)
			{
				continue;
			}
			else if (tokens.size() != 3)
			{
				WRITE_LOG(logger, Log::warning,
				          "Config::loadFromFile() failed at line " << lineCount
				       << ": wrong number of tokens" << std::endl);
				continue;
			}

			auto it{tokens.begin()};
			const std::string &key{*it};
			++it;
			const std::string &op{*it};
			++it;
			const std::string &value{*it};
			if (op != "=")
			{
				WRITE_LOG(logger, Log::warning,
				          "Config::loadFromFile() failed at line " << lineCount
				       << ": wrong operator" << std::endl);
				continue;
			}

			set(key, value);

			++lineCount;
		}

		return true;
	}

	template <typename T>
	bool get(const std::string &key, T &target) const
	{
		std::stringstream valueStream;
		try
		{
			valueStream = std::stringstream{db.at(key)};
		}
		catch (std::out_of_range &exception)
		{
			WRITE_LOG(logger, Log::warning,
			          "Config::get(): key \"" << key << "\" does not exist" << std::endl);
			return false;
		}

		valueStream >> target;
		if (valueStream.fail())
		{
			WRITE_LOG(logger, Log::warning,
			          "Config::get() failed to access \"" << key
			       << "\" as \"" << typeid(T).name() << "\"" << std::endl);
			return false;
		}

		return true;
	}
};

#endif // ifndef CONFIG_HPP

