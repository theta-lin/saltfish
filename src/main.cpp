#include <iostream>
#include <cstring>
#include "SDL.h"
#include "SDL_main.h"
#include "video.hpp"
#include "surface.hpp"
#include "line.hpp"
#include "cross.hpp"

int main(int argc, char *argv[])
{
	try
	{
		checkCompatibility();
		Video video{"saltfish", 640, 480};

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
		std::cerr << "Caught std::runtime_error: " << exception.what() << std::endl;
	}

	return 0;
}

