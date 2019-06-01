#include "video.hpp"

Video::Video(Log &logger) : logger{logger}, window{nullptr}
{
}

Video::Video(Log &logger, const std::string &title, int width, int height) : logger{logger}, window{nullptr}
{
	init(title, width, height);
}

Video::~Video()
{
	if (window)
		cleanup();
}

void Video::init(const std::string &title, int width, int height)
{
	if (window)
		throw std::runtime_error("Video::init() failed: window already exist");

	logger.lock();
	logger.GET(LogLevel::info) << "Initialize video mode: " << width << 'x' << height << std::endl;
	logger.unlock();

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		std::string message{"SDL_Init() Error: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}

	window = SDL_CreateWindow(title.c_str() , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (!window)
	{
		std::string message{"SDL_CreateWindow() Error: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}

	surface = SDL_GetWindowSurface(window);
	if (!surface)
	{
		std::string message{"SDL_GetWindowSurface() Error: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
	surface.setManaged(false);
}

void Video::cleanup()
{
	logger.lock();
	logger.GET(LogLevel::info) << "Cleanup video" << std::endl;
	logger.unlock();

	if (!window)
		throw std::runtime_error("Video::cleanup() failed: window is nullptr");
	SDL_DestroyWindow(window);
	SDL_Quit();
}

Surface& Video::getSurface()
{
	if (!window)
		throw std::runtime_error("Video::getSurface() failed: window is nullptr");
	return surface;
}

void Video::update()
{
	if (!window)
		throw std::runtime_error("Video::update() failed: window is nullptr");
	if (SDL_UpdateWindowSurface(window) < 0)
	{
		std::string message{"Video::update() failed: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

Video::operator bool()
{
	if (window)
		return true;
	else
		return false;
}

