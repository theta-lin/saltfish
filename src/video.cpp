#include "video.hpp"

bool Video::setTitle(const std::string &title)
{
	if (inited)
	{
		std::cerr << "Failed to set title, already started video" << std::endl;
		return false;
	}

	this->title = title;
	return true;
}

bool Video::setVideoMode(uint32_t width, uint32_t height)
{
	if (inited)
	{
		std::cerr << "Failed to set video mode, already started video" << std::endl;
		return false;
	}

	this->width = width;
	this->height = height;
	return true;
}

bool Video::init()
{
	cleanup();

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return false;
	}

	window = SDL_CreateWindow(title.c_str() , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (window == nullptr)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr)
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return false;
	}

	inited = true;
	return true;
}

void Video::cleanup()
{
	if (inited)
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		inited = false;
	}
}

