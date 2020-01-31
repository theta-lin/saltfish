#ifndef LOG_HPP
#define LOG_HPP

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <functional>
#include <iomanip>
#include <mutex>
#include <ostream>
#include <string>
#include <vector>

/*
 * A dummy buffer to pipe into if you want to have no output
 * similar to /dev/null, etc.
 */
class NullBuffer : public std::streambuf
{
public:
	using int_type = std::streambuf::int_type;
	int_type overflow(int_type ch) final;
};

/*
 * A buffer that forwards output to multiple ostream,
 * so you can output to file and stdout at the same time.
 */
class ComposedBuffer : public std::streambuf
{
private:
	std::vector<std::streambuf*> bufferList;

public:
	using int_type = std::streambuf::int_type;

	void add(std::streambuf *buffer);
	int_type overflow(int_type ch) final;
};

/*
 * A class that allows logging to multiple ostream,
 * also suppress low-importance message according to Log Level,
 * append information like Log Level, file name, line number, and current time.
 */
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
	Level level; // suppress message with Level number GREATER than this
	static std::array<std::string, 4> levelToString;

public:
	Log(Level level);
	void lock();
	void unlock();
	void bind(std::ostream &observer);

	/*
	 * Return a std::ostream to directly write into
	 * RECOMMAND USING THE MACRO WRAPPER
	 */
	std::ostream& get(Level level, std::string fileName, int lineNumber);
};

/*
 * An easy macro wrapper for Log::get(),
 * inserts current file name and line number,
 * also HANDLES LOCKING,
 * MESSAGE should be connected with << operator.
 */
#define WRITE_LOG(LOGGER, LEVEL, MESSAGE) \
		( \
			(LOGGER.lock()), \
			(LOGGER.get(LEVEL, std::filesystem::path{__FILE__}.filename(), __LINE__) << MESSAGE), \
			(LOGGER.unlock()) \
		)


#endif // ifndef LOG_HPP

