#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include <stdexcept>
#include "SDL.h"
#include "surface.hpp"

class Video
{
private:
	SDL_Window *window;
	Surface surface;

public:
	Video();
	Video(const std::string &title, int width, int height);
	~Video();

	void init(const std::string &title, int width, int height);
	void cleanup();
	Surface& getSurface();
	void update();
	operator bool();
};

#endif // ifndef VIDEO_HPP

