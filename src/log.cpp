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

std::string stringOfLogLevel(LogLevel level)
{
	static const std::string lookup[4] = {"ERROR", "WARNING", "INFO", "DEBUG"};
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

std::ostream& Log::get(LogLevel level, const std::string &functionName)
{
	if (level <= this->level)
	{
		auto now{std::chrono::system_clock::now()};
		auto in_time_t{std::chrono::system_clock::to_time_t(now)};
		out << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S ")
			<< '[' << stringOfLogLevel(level) << "] " << functionName << ": ";
		return out;
	}
	else
	{
		return nullOut;
	}
}

NullBuffer Log::nullBuffer{};
std::ostream Log::nullOut{&Log::nullBuffer};

