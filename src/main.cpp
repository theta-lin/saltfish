#include <iostream>
#include <algorithm>
#include <vector>
#include <cstring>
#include "SDL.h"

const int g_screenWidth{640};
const int g_screenHeight{480};

int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_Window *window{SDL_CreateWindow("saltfish", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, g_screenWidth, g_screenHeight, 0)};
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
	}

	SDL_Renderer *renderer{SDL_CreateRenderer(window, -1, 0)};
	if (renderer == nullptr)
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	SDL_Texture *texture{SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STATIC, g_screenWidth, g_screenHeight)};
	if(texture == nullptr)
	{
		std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
		SDL_DestroyTexture(texture);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	std::vector<Uint32> pixels(g_screenWidth * g_screenHeight);
	void *pixelsPtr{nullptr};
	int pitch;

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

		SDL_LockTexture(texture, 0, &pixelsPtr, &pitch);
		SDL_UpdateTexture(texture, 0, pixels.data(), g_screenWidth * sizeof(Uint32));
		SDL_UnlockTexture(texture);

		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, texture, 0, 0);
		SDL_RenderPresent(renderer);

	}

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
