#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <SDL.h>
#include "log.hpp"
#include "config.hpp"
#include "surface.hpp"

namespace sw
{

class Window
{
private:
	Log &logger;
	SDL_Window *window;
	Surface surface;

public:
	Window(Log &logger, const std::string &title, const Config &config);
	~Window();

	void init(const std::string &title, const Config &config);
	void cleanup();
	Surface& getSurface();
	void update();
	operator bool();
};

} // namespace sw

#endif // ifndef VIDEO_HPP

