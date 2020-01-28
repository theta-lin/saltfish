#ifndef VIDEO_HPP
#define VIDEO_HPP

#include "config.hpp"
#include "log.hpp"
#include "surface.hpp"
#include <SDL.h>

namespace sw // Sdl Wrapper
{

/*
 * A simple wrapper for SDL_Window
 */
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

