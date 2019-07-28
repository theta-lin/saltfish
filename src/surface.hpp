#ifndef SURFACE_HPP
#define SURFACE_HPP

#include <string>
#include <stdexcept>
#include <SDL.h>

class PixelView
{
private:
	Uint8 *const pixel;
	const SDL_PixelFormat *const format;

public:
	PixelView(Uint8 *const pixel, const SDL_PixelFormat *const format);
	PixelView(const PixelView &pixelView) = delete;
	PixelView(PixelView &&pixelView) = delete;

	operator Uint32() const;
	operator SDL_Color() const;
	PixelView& operator=(const SDL_Color &color);
	PixelView& operator=(const PixelView &pixelView);
};

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
	bool getMustLock();
	bool getManaged();

	Surface convert(const SDL_PixelFormat *fmt);
	void blitSurface(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	void blitScaled(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect);
	void fillRect(const SDL_Rect *rect, const SDL_Color &color);

	void lock();
	void unlock();
	void setRLE(bool flag);
	void setManaged(bool flag);

	Surface& operator=(Surface &&surface);
	operator bool();
	PixelView operator[](int index);
	PixelView operator()(int row, int col);
};

#endif // ifndef SURFACE_HPP

