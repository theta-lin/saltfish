#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include "SDL.h"
#include "log.hpp"
#include "surface.hpp"

class Video
{
private:
	Log &logger;
	SDL_Window *window;
	Surface surface;

public:
	Video(Log &logger);
	Video(Log &logger, const std::string &title, int width, int height);
	~Video();

	void init(const std::string &title, int width, int height);
	void cleanup();
	Surface& getSurface();
	void update();
	operator bool();
};

#endif // ifndef VIDEO_HPP

