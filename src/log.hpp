#ifndef LOG_HPP
#define LOG_HPP

#include <ostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <string>
#include <array>
#include <chrono>
#include <iomanip>
#include <mutex>
#include <filesystem>

class NullBuffer : public std::streambuf
{
public:
	using int_type = std::streambuf::int_type;
	int_type overflow(int_type ch) final;
};

class ComposedBuffer : public std::streambuf
{
private:
	std::vector<std::streambuf*> bufferList;

public:
	using int_type = std::streambuf::int_type;

	void add(std::streambuf *buffer);
	int_type overflow(int_type ch) final;
};

class Log
{
public:
	enum Level
	{
		error = 0,
		warning = 1,
		info = 2,
		debug = 3
	};

private:
	std::mutex mutex;
	ComposedBuffer buffer;
	std::ostream out;
	static NullBuffer nullBuffer;
	static std::ostream nullOut;
	Level level;
	static std::array<std::string, 4> levelToString;

public:
	Log(Level level);
	void lock();
	void unlock();
	void setLevel(Level level);
	Level getLevel();
	void bind(std::ostream &observer);

	/*
	 * Return a std::ostream to directly log to
	 * RECOMMAND USING THE MACRO WRAPPER
	 */
	std::ostream& get(Level level, std::string fileName, int lineNumber);
};

/*
 * An easy macro wrapper for Log::get()
 * Inserts current function name
 */
#define GET(LEVEL)\
		get(LEVEL, std::filesystem::path{__FILE__}.filename(), __LINE__)

#endif // ifndef LOG_HPP

