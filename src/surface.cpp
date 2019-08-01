#include "surface.hpp"

namespace sw
{

Surface::Surface(SDL_Surface *surface) : surface{surface}, managed{true}
{
}

Surface::Surface(Surface &&surface) : surface{surface.getPtr()}, managed{surface.getManaged()}
{
	if (surface.getPtr())
		surface.free();
}

Surface::Surface(int width, int height, int depth, Uint32 format) : surface{nullptr}, managed{true}
{
	create(width, height, depth, format);
}

Surface::Surface(void *pixels, int width, int height, int depth, int pitch, Uint32 format) : surface{nullptr}, managed{true}
{
	create(pixels, width, height, depth, pitch, format);
}

Surface::~Surface()
{
	if (surface && managed)
		free();
}

void Surface::create(int width, int height, int depth, Uint32 format)
{
	if (surface)
		throw std::runtime_error{"Surface::create() failed: surface already exist"};

	surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
	if (!surface)
	{
		std::string message{"Surface::create() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::create(void *pixels, int width, int height, int depth, int pitch, Uint32 format)
{
	if (surface)
		throw std::runtime_error{"Surface::create() failed: surface already exist"};

	surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, pitch, format);
	if (!surface)
	{
		std::string message{"Surface::create() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::free()
{
	if (!surface)
		throw std::runtime_error{"Surface::free() failed: surface is nullptr"};
	SDL_FreeSurface(surface);
	surface = nullptr;
}

SDL_Surface* Surface::getPtr()
{
	return surface;
}

int Surface::getWidth()
{
	if (!surface)
		throw std::runtime_error{"Surface::getWidth() failed: surface is nullptr"};
	return surface->w;
}

int Surface::getHeight()
{
	if (!surface)
		throw std::runtime_error{"Surface::getHeight() failed: surface is nullptr"};
	return surface->h;
}

int Surface::getPitch()
{
	if (!surface)
		throw std::runtime_error{"Surface::getPitch() failed: surface is nullptr"};
	return surface->pitch;
}

void* Surface::getPixels()
{
	if (!surface)
		throw std::runtime_error{"Surface::getPixels() failed: surface is nullptr"};
	return surface->pixels;
}

SDL_PixelFormat* Surface::getFormat()
{
	if (!surface)
		throw std::runtime_error{"Surface::getFormat() failed: surface is nullptr"};
	return surface->format;
}

bool Surface::getMustLock()
{
	if (!surface)
		throw std::runtime_error{"Surface::getMustLock() failed: surface is nullptr"};
	return SDL_MUSTLOCK(surface);
}

bool Surface::getManaged()
{
	return managed;
}

Surface Surface::convert(const SDL_PixelFormat *fmt)
{
	if (!surface)
		throw std::runtime_error{"Surface::convert() failed: surface is nullptr"};

	SDL_Surface *temp{SDL_ConvertSurface(surface, fmt, 0)};
	if (!temp)
	{
		std::string message{"Surface::convert() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}

	return temp;
}

void Surface::blitSurface(Surface &dst, const Rect *srcrect, Rect *dstrect)
{
	if (!surface)
		throw std::runtime_error{"Surface::blitSurface() failed: surface is nullptr"};

	if (SDL_BlitSurface(surface, srcrect, dst.surface, dstrect) < 0)
	{
		std::string message{"Surface::blitSurface() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::blitScaled(Surface &dst, const Rect *srcrect, Rect *dstrect)
{
	if (!surface)
		throw std::runtime_error{"Surface::blitScaled() failed: surface is nullptr"};

	if (SDL_BlitScaled(surface, srcrect, dst.surface, dstrect) < 0)
	{
		std::string message{"Surface::blitScaled() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::fillRect(const Rect *rect, const Color &color)
{
	if (!surface)
		throw std::runtime_error{"Surface::fillRect() failed: surface is nullptr"};

	if (SDL_FillRect(surface, rect, SDL_MapRGBA(getFormat(), color.r, color.g, color.b, color.a)) < 0)
	{
		std::string message{"Surface::fillRect() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::lock()
{
	if (!surface)
		throw std::runtime_error{"Surface::lock() failed: surface is nullptr"};

	if (SDL_LockSurface(surface) < 0)
	{
		std::string message{"Surface::lock() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::unlock()
{
	if (!surface)
		throw std::runtime_error{"Surface::unlock() failed: surface is nullptr"};
	SDL_UnlockSurface(surface);
}

void Surface::setRLE(bool flag)
{
	if (!surface)
		throw std::runtime_error{"Surface::setRLE() failed: surface is nullptr"};

	if (SDL_SetSurfaceRLE(surface, flag) < 0)
	{
		std::string message{"Surface::setRLE() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}
}

void Surface::setManaged(bool flag)
{
	managed = flag;
}

Surface& Surface::operator=(Surface &&surface)
{
	if (this != &surface)
	{
		if (this->surface)
			free();
		this->surface = surface.surface;
		this->managed = surface.managed;
		surface.surface = nullptr;
	}

	return *this;
}

Surface::operator bool()
{
	if (surface)
		return true;
	else
		return false;
}

PixelView Surface::operator[](int index)
{
	return {static_cast<Uint8*>(getPixels()) + index * getFormat()->BytesPerPixel,
			getFormat()};
}

PixelView Surface::operator()(int row, int col)
{
	return {static_cast<Uint8*>(getPixels()) + row * getPitch() + col * getFormat()->BytesPerPixel,
			getFormat()};
}

} // namespace sw

