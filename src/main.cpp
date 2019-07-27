#include <cstring>
#include <iostream>
#include <stdexcept>
#include <filesystem>
#include <cassert>
#include <limits>
#include "SDL.h"
#include "SDL_main.h"
#include "log.hpp"
#include "config.hpp"
#include "video.hpp"
#include "surface.hpp"
#include "line.hpp"

int main(int argc, char *argv[])
{
	static_assert(sizeof(double) == 8, "Require size of double to be 8");
	static_assert(std::numeric_limits<double>::is_iec559, "Require IEEE 559 for double");
	static_assert(std::numeric_limits<double>::has_infinity, "Require infinity for double");
	static_assert(std::numeric_limits<double>::has_quiet_NaN, "Require quit NaN for double");

	try
	{
		Log logger{LogLevel::debug};
		logger.bind(std::cout);
		logger.GET(LogLevel::info) << "Started program" << std::endl;

		assert(argc != 0);
		namespace fs = std::filesystem;
		fs::path exeDir{fs::current_path() / fs::path{argv[0]}.parent_path()};

		Config config{logger};
		if (!config.loadFromFile(exeDir / "saltfish.conf"))
			throw std::runtime_error{"FATAL: cannot open config file"};

		Video video{logger, "saltfish", config};
		Line l1{{50, 50}, {50, 200}};

		bool quit{false};
		SDL_Event event;
		while(!quit)
		{
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_QUIT:
						quit = true;
						break;
				}
			}

			video.getSurface().fillRect(nullptr, SDL_Color{0, 0, 0, 255});
			l1.draw({255, 255, 0, 0}, video.getSurface());
			video.update();
		}
	}
	catch(const std::runtime_error &exception)
	{
		logger.lock();
		logger.GET(LogLevel::error) << "Caught std::runtime_error: " << exception.what() << std::endl;
		logger.unlock();
	}
	catch(...)
	{
		logger.lock();
		logger.GET(LogLevel::error) << "Caught Unknown Exception" << std::endl;
		logger.unlock();
	}

	return 0;
}

