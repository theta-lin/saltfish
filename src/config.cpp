#include "config.hpp"

void Config::add(const std::string &key, ConfigType type)
{
	if (db.count(key) > 0)
		throw std::runtime_error{"Config::add() failed: key already exist"};

	db[key].first = type;
}

void Config::set(const std::string &key, const std::string &value)
{
	if (db.count(key) == 0)
		throw std::runtime_error{"Config::parse() failed: key does not exist"};

	std::stringstream valueStream{value};
	switch (db[key].first)
	{
	case ConfigType::INT:
	{
		int temp;
		valueStream >> temp;
		if (!valueStream)
			throw std::runtime_error{"Config::parse() failed to input value of type int"};
		db[key].second = temp;
		break;
	}
	case ConfigType::DOUBLE:
	{
		double temp;
		valueStream >> temp;
		if (!valueStream)
			throw std::runtime_error{"Config::parse() failed to input value of type double"};
		db[key].second = temp;
		break;
	}
	default:
		throw std::runtime_error{"Config::parse() failed to select ConfigType"};
	}
}

void Config::loadFromFile(const std::string &fileName)
{
	std::ifstream inFile{fileName};
	if (!inFile)
	{
		std::string message{"Config::loadFromFile() failed: unable to open \""};
		message += (fileName);
		message += "\"";
		throw std::runtime_error{message};
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
			std::string message{"Config::loadFromFile() failed at line "};
			message += std::to_string(lineCount);
			message += ": wrong number of tokens";
			throw std::runtime_error{message};
		}

		auto it{tokens.begin()};
		const std::string &key{*it};
		++it;
		const std::string &op{*it};
		++it;
		const std::string &value{*it};
		if (op != "=")
		{
			std::string message{"Config::loadFromFile() failed at line "};
			message += std::to_string(lineCount);
			message += ": wrong operator";
			throw std::runtime_error{message};
		}

		try
		{
			set(key, value);
		}
		catch(const std::runtime_error &error)
		{
			std::string message{"Config::loadFromFile() failed at line "};
			message += std::to_string(lineCount);
			message += ":\n\t";
			message += error.what();
			throw std::runtime_error{message};
		}

		++lineCount;
	}
}

void Config::get(const std::string &key, int &target)
{
	try
	{
		target = std::get<int>(db[key].second);
	}
	catch (const std::bad_variant_access &error)
	{
		std::string message("Config::get() failed to access \"");
		message += key;
		message += "\" as int:\n\t";
		message += error.what();
	}
}

void Config::get(const std::string &key, double &target)
{
	try
	{
		target = std::get<double>(db[key].second);
	}
	catch (const std::bad_variant_access &error)
	{
		std::string message("Config::get() failed to access \"");
		message += key;
		message += "\" as double:\n\t";
		message += error.what();
	}
}
