#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include <SDL.h>
#include "log.hpp"
#include "config.hpp"
#include "surface.hpp"

class Video
{
private:
	Log &logger;
	SDL_Window *window;
	Surface surface;

public:
	Video(Log &logger);
	Video(Log &logger, const std::string &title, const Config &config);
	~Video();

	void init(const std::string &title, const Config &config);
	void cleanup();
	Surface& getSurface();
	void update();
	operator bool();
};

#endif // ifndef VIDEO_HPP

