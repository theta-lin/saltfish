#include "log.hpp"

NullBuffer::int_type NullBuffer::overflow(int_type ch)
{
	return ch;
}

void ComposedBuffer::add(std::streambuf *buffer)
{
	bufferList.push_back(buffer);
}

ComposedBuffer::int_type ComposedBuffer::overflow(int_type ch)
{
	std::for_each(bufferList.begin(), bufferList.end(),
				  [ch](std::streambuf* buffer)
				  {
						std::mem_fn(&std::streambuf::sputc)(*buffer, ch);
				  });
	return ch;
}

std::string_view stringOfLogLevel(LogLevel level)
{
	static const std::array<std::string, 4> lookup{"ERROR", "WARNING", "INFO", "DEBUG"};
	return lookup[static_cast<int>(level)];
}

Log::Log(LogLevel level) : out{&buffer}, level{level}
{
}

void Log::lock()
{
	mutex.lock();
}

void Log::unlock()
{
	mutex.unlock();
}

void Log::setLevel(LogLevel level)
{
	this->level = level;
}

LogLevel Log::getLevel()
{
	return level;
}

void Log::bind(std::ostream &observer)
{
	buffer.add(observer.rdbuf());
}

std::ostream& Log::get(LogLevel level, std::string fileName, int lineNumber)
{
	if (level <= this->level)
	{
		auto now{std::chrono::system_clock::now()};
		auto in_time_t{std::chrono::system_clock::to_time_t(now)};
		out << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S ")
			<< '[' << stringOfLogLevel(level) << "] "
			<< fileName << ":" << lineNumber << ": ";
		return out;
	}
	else
	{
		return nullOut;
	}
}

NullBuffer Log::nullBuffer{};
std::ostream Log::nullOut{&Log::nullBuffer};

