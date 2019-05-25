#ifndef SURFACE
#define SURFACE

#include <string>
#include <stdexcept>
#include "SDL.h"

class Surface
{
private:
	SDL_Surface *surface;

public:
	Surface(SDL_Surface *surface = nullptr);
	Surface(Surface &surface) = delete;
	Surface(Surface &&surface);
	Surface(int width, int height, int depth = 32, Uint32 format = SDL_PIXELFORMAT_RGBA32, void *pixels = nullptr);
	~Surface();

	void create(int width, int height, int depth = 32, Uint32 format = SDL_PIXELFORMAT_RGBA32, void *pixels = nullptr);
	void free();

	SDL_Surface* getPtr();
	int getWidth();
	int getHeight();
	int getPitch();
	void* getPixels();
	SDL_PixelFormat* getFormat();
	bool getMustLock();

	Surface convert(const SDL_PixelFormat *fmt);
	void blitSurface(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	void blitScaled(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	void lock();
	void unlock();
	void setRLE(bool flag);

	Surface& operator=(Surface &&surface);

};

#endif // ifndef surface

