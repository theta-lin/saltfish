#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <map>
#include <string>
#include <sstream>
#include <fstream>
#include "log.hpp"
#include "io.hpp"

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
			logger.lock();
			logger.GET(LogLevel::error)
				<< "Config::loadFromFile() failed: unable to open \""
				<< fileName << "\"" << std::endl;
			logger.unlock();
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
				logger.lock();
				logger.GET(LogLevel::warning)
					<< "Config::loadFromFile() failed at line " << lineCount
					<< ": wrong number of tokens" << std::endl;
				logger.unlock();
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
				logger.lock();
				logger.GET(LogLevel::warning)
					<< "Config::loadFromFile() failed at line " << lineCount
				    << ": wrong operator" << std::endl;
				logger.unlock();
				continue;
			}

			set(key, value);

			++lineCount;
		}

		return true;
	}

	template <typename T>
	bool get(const std::string &key, T &target)
	{
		std::stringstream valueStream{db[key]};
		valueStream >> target;
		if (valueStream.fail())
		{
			logger.GET(LogLevel::warning)
				<< "Config::get() failed to access \"" << key
				<< "\" as \"" << typeid(T).name() << "\"" << std::endl;
			return false;
		}
		else
		{
			return true;
		}
	}
};

#endif // ifndef CONFIG_HPP
