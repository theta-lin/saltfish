#include <iostream>
#include <cstring>
#include "SDL.h"
#include "video.hpp"
#include "surface.hpp"
#include "line.hpp"
#include "cross.hpp"

int main(int argc, char *argv[])
{
	checkCompatibility();

	Video video{"saltfish", 640, 480};
	if (!video.init())
	{
		return 1;
	}

	SDL_Texture *texture{SDL_CreateTexture(video.getRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, video.getWidth(), video.getHeight())};
	if(texture == nullptr)
	{
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

	try
	{
		Surface surface{video.getWidth(), video.getHeight()};
		memset(surface.getPixels(), 0, surface.getWidth() * surface.getHeight() * surface.getFormat()->BytesPerPixel);
		void *pixelsPtr{nullptr};
		int pitch;
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

			l1.draw({255, 255, 255, 255}, surface);

			SDL_LockTexture(texture, 0, &pixelsPtr, &pitch);
			SDL_UpdateTexture(texture, 0, surface.getPixels(), video.getWidth() * sizeof(Uint32));
			SDL_UnlockTexture(texture);

			SDL_SetRenderDrawColor(video.getRenderer(), 0, 0, 0, 255);
			SDL_RenderClear(video.getRenderer());
			SDL_RenderCopy(video.getRenderer(), texture, 0, 0);
			SDL_RenderPresent(video.getRenderer());
		}
	}
	catch(const std::runtime_error &exception)
	{
		std::cerr << "Caught std::runtime_error: " << exception.what() << std::endl;
	}

	SDL_DestroyTexture(texture);
	return 0;
}

