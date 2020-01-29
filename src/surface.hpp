#ifndef SURFACE_HPP
#define SURFACE_HPP

#include "pixels.hpp"

namespace sw // Sdl Wrapper
{

/*
 * A simple wrapper for SDL_Surface
 * NOTE: Due to the specification of SDL, const object will be unavailable
 */
class Surface
{
private:
	SDL_Surface *surface;
	bool managed;

public:
	Surface(SDL_Surface *surface = nullptr);
	Surface(Surface &surface) = delete;
	Surface(Surface &&surface);
	Surface(int width, int height, int depth = 32, Uint32 format = SDL_PIXELFORMAT_RGBA32);
	Surface(void *pixels, int width, int height, int depth, int pitch, Uint32 format);
	~Surface();

	void create(int width, int height, int depth = 32, Uint32 format = SDL_PIXELFORMAT_RGBA32);
	void create(void *pixels, int width, int height, int depth, int pitch, Uint32 format);
	void free();

	SDL_Surface* getPtr();
	int getWidth();
	int getHeight();
	int getPitch();
	void* getPixels();
	SDL_PixelFormat* getFormat();

	Surface convert(Uint32 pixel_format);
	void blit(Surface &dst, const Rect *srcrect, Rect *dstrect);
	void blitScaled(Surface &dst, const Rect *srcrect, Rect *dstrect);
	void fillRect(const Rect *rect, const Color &color);
	bool setClipRect(const Rect *rect);
	void getClipRect(Rect *rect);

	void lock();
	void unlock();
	bool getMustLock();
	void setRLE(bool flag);
	void setManaged(bool flag);
	bool getManaged();

	Surface& operator=(Surface &&surface);
	operator bool();
	PixelView operator[](int index);
	PixelView operator()(int col, int row);
};

} // namespace sw

#endif // ifndef SURFACE_HPP

