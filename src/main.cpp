#include <iostream>
#include <cstring>
#include "SDL.h"
#include "SDL_main.h"
#include "log.hpp"
#include "config.hpp"
#include "video.hpp"
#include "surface.hpp"
#include "line.hpp"
#include "cross.hpp"

int main(int argc, char *argv[])
{
	Log logger{LogLevel::debug};
	logger.bind(std::cout);

	logger.lock();
	logger.GET(LogLevel::info) << "Started program" << std::endl;
	logger.unlock();

	checkCompatibility();
	logger.lock();
	logger.GET(LogLevel::debug) << "Compatiblity check passed" << std::endl;
	logger.unlock();

	try
	{
		Config config;
		config.add("window.width", ConfigType::INT);
		config.add("window.height", ConfigType::INT);
		config.loadFromFile("saltfish.conf");
		int windowWidth, windowHeight;
		config.get("window.width", windowWidth);
		config.get("window.height", windowHeight);

		Video video{logger, "saltfish", windowWidth, windowHeight};
		video.getSurface().fillRect(nullptr, SDL_Color{0, 0, 0, 255});

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

			l1.draw({255, 255, 255, 255}, video.getSurface());
			video.update();
		}
	}
	catch(const std::runtime_error &exception)
	{
		logger.lock();
		logger.GET(LogLevel::error) << "Caught std::runtime_error: " << exception.what() << std::endl;
		logger.unlock();
	}

	return 0;
}

