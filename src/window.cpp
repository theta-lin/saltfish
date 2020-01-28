#include "window.hpp"

namespace sw // Sdl Wrapper
{

Window::Window(Log &logger, const std::string &title, const Config &config) : logger{logger}, window{nullptr}
{
	init(title, config);
}

Window::~Window()
{
	cleanup();
}

void Window::init(const std::string &title, const Config &config)
{
	if (window)
		throw std::runtime_error{"Window::init() failed: window already exist"};

	int width{640}, height{480};
	config.get("window.width", width);
	config.get("window.height", height);

	logger.lock();
	logger.GET(Log::info) << "Initialize video mode: " << width << 'x' << height << std::endl;
	logger.unlock();

	window = SDL_CreateWindow(title.c_str() , SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
	if (!window)
	{
		std::string message{"SDL_CreateWindow() Error: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}

	surface = SDL_GetWindowSurface(window);
	if (!surface)
	{
		std::string message{"SDL_GetWindowSurface() Error: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
	surface.setManaged(false);
}

void Window::cleanup()
{
	logger.lock();
	logger.GET(Log::info) << "Cleanup video" << std::endl;
	logger.unlock();

	if (!window)
		throw std::runtime_error{"Window::cleanup() failed: window is nullptr"};
	SDL_DestroyWindow(window);
}

Surface& Window::getSurface()
{
	if (!window)
		throw std::runtime_error{"Window::getSurface() failed: window is nullptr"};
	return surface;
}

void Window::update()
{
	if (!window)
		throw std::runtime_error{"Window::update() failed: window is nullptr"};
	if (SDL_UpdateWindowSurface(window) < 0)
	{
		std::string message{"Window::update() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

Window::operator bool()
{
	if (window)
		return true;
	else
		return false;
}

} // namespace sw

