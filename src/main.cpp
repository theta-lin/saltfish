#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <limits>
#include <SDL.h>
#include <SDL_ttf.h>
#include "log.hpp"
#include "config.hpp"
#include "window.hpp"
#include "surface.hpp"
#include "font.hpp"
#include "line.hpp"
#include "program.hpp"
#include "event.hpp"
#include "game.hpp"

int main(int argc, char *argv[])
{
	static_assert(sizeof(double) == 8, "Requires size of double to be 8");
	static_assert(std::numeric_limits<double>::is_iec559, "Requires IEEE 559 for double");
	static_assert(std::numeric_limits<double>::has_infinity, "Requires infinity for double");
	static_assert(std::numeric_limits<double>::has_quiet_NaN, "Requires quiet NaN for double");

	Log logger{Log::debug};
	logger.bind(std::clog);

	logger.lock();
	logger.GET(Log::info) << "Started program" << std::endl;

	SDL_version SDLCompiled;
	SDL_VERSION(&SDLCompiled);
	logger.GET(Log::info) << "SDL compiled Version: "
		<< static_cast<int>(SDLCompiled.major) << '.'
		<< static_cast<int>(SDLCompiled.minor) << '.'
		<< static_cast<int>(SDLCompiled.patch) << std::endl;

	SDL_version SDLLinked;
	SDL_GetVersion(&SDLLinked);
		logger.GET(Log::info) << "SDL linked Version: "
		<< static_cast<int>(SDLLinked.major) << '.'
		<< static_cast<int>(SDLLinked.minor) << '.'
		<< static_cast<int>(SDLLinked.patch) << std::endl;

	SDL_version TTFCompiled;
	SDL_TTF_VERSION(&TTFCompiled);
	logger.GET(Log::info) << "SDL_ttf compiled Version: "
		<< static_cast<int>(TTFCompiled.major) << '.'
		<< static_cast<int>(TTFCompiled.minor) << '.'
		<< static_cast<int>(TTFCompiled.patch) << std::endl;

	const SDL_version *TTFLinked{TTF_Linked_Version()};
	logger.GET(Log::info) << "SDL_ttf linked Version: "
		<< static_cast<int>(TTFLinked->major) << '.'
		<< static_cast<int>(TTFLinked->minor) << '.'
		<< static_cast<int>(TTFLinked->patch) << std::endl;
	logger.unlock();

	assert(argc != 0);
	namespace fs = std::filesystem;
	fs::path exeDir{fs::current_path() / fs::path{argv[0]}.parent_path()};

	//Game game{logger, exeDir};
	//game.loadLevel("test.lvl");
	//game.print();
	//return 0;

	try
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
		{
			std::string message{"SDL_Init() Error: "};
			message += SDL_GetError();
			throw std::runtime_error{message};
		}
		if (std::atexit(SDL_Quit) != 0)
			throw std::runtime_error{"Registration of SDL_Quit() failed"};
		logger.lock();
		logger.GET(Log::info) << "Initialized SDL" << std::endl;
		logger.unlock();

		if (TTF_Init() == -1)
		{
			std::string message{"TTF_Init() Error: "};
			message += TTF_GetError();
			throw std::runtime_error{message};
		}
		if (std::atexit(TTF_Quit) != 0)
			throw std::runtime_error{"Registration of TTF_Quit() failed"};
		logger.lock();
		logger.GET(Log::info) << "Initialized SDL_ttf" << std::endl;
		logger.unlock();

		Config config{logger};
		if (!config.loadFromFile(exeDir / "saltfish.conf"))
			throw std::runtime_error{"FATAL: cannot open config file"};

		sw::Window window{logger, "saltfish", config};
		Program program{logger, exeDir, window.getSurface()};

		sw::Event event;
		while(!program.isExited())
		{
			while (SDL_PollEvent(&event))
				program.handleEvent(event);
			program.update();
			window.update();
		}
	}
	catch(const std::runtime_error &exception)
	{
		logger.lock();
		logger.GET(Log::error) << "Caught std::runtime_error: " << exception.what() << std::endl;
		logger.unlock();
		return 1;
	}
	catch(...)
	{
		logger.lock();
		logger.GET(Log::error) << "Caught Unknown Exception" << std::endl;
		logger.unlock();
		return 1;
	}

	return 0;
}

