#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <iostream>
#include <string>
#include "SDL.h"
#include "color.hpp"
#include "memory.hpp"
#include "line.hpp"

class Video
{
private:
	bool inited;
	std::string title;
	int width;
	int height;

	SDL_Window *window;
	SDL_Renderer *renderer;

public:
	Video() : inited{false}, width{0}, height{0}, window{nullptr}, renderer{nullptr}
	{
	}

	Video(const std::string &title, int width, int height) : inited{false}, title{title}, width{width}, height{height}, window{nullptr}, renderer{nullptr}
	{
	}

	~Video()
	{
		cleanup();
	}

	bool isInited() { return inited; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	SDL_Renderer* getRenderer() { return renderer; }

	bool setTitle(const std::string &title);
	bool setVideoMode(int width, int height);
	bool init();
	void cleanup();
};

#endif // ifndef VIDEO_HPP

