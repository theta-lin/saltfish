#include "surface.hpp"

namespace sw // Sdl_Wrapper
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

Surface Surface::convert(Uint32 pixel_format)
{
	if (!surface)
		throw std::runtime_error{"Surface::convert() failed: surface is nullptr"};

	SDL_Surface *temp{SDL_ConvertSurfaceFormat(surface, pixel_format, 0)};
	if (!temp)
	{
		std::string message{"Surface::convert() failed: "};
		message += SDL_GetError();
		throw std::runtime_error{message};
	}

	return temp;
}

void Surface::blit(Surface &dst, const Rect *srcrect, Rect *dstrect)
{
	if (!surface)
		throw std::runtime_error{"Surface::blit() failed: surface is nullptr"};

	if (SDL_BlitSurface(surface, srcrect, dst.surface, dstrect) < 0)
	{
		std::string message{"Surface::blit() failed: "};
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

bool Surface::setClipRect(const Rect *rect)
{
	return static_cast<bool>(SDL_SetClipRect(surface, rect));
}

void Surface::getClipRect(Rect *rect)
{
	SDL_GetClipRect(surface, rect);
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

bool Surface::getMustLock()
{
	if (!surface)
		throw std::runtime_error{"Surface::getMustLock() failed: surface is nullptr"};
	return static_cast<bool>(SDL_MUSTLOCK(surface));
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

bool Surface::getManaged()
{
	return managed;
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

PixelView Surface::operator()(int col , int row)
{
	return {static_cast<Uint8*>(getPixels()) + col * getFormat()->BytesPerPixel + row * getPitch(),
			getFormat()};
}

} // namespace sw

