#include "surface.hpp"

Surface::Surface(SDL_Surface *surface) : surface{surface}
{
}

Surface::Surface(Surface &&surface) : surface{surface.getPtr()}
{
	if (surface.getPtr())
		surface.free();
}

Surface::Surface(int width, int height, int depth, Uint32 format, void *pixels)
{
	create(width, height, depth, format, pixels);
}

Surface::~Surface()
{
	if (surface)
		free();
}

void Surface::create(int width, int height, int depth, Uint32 format, void *pixels)
{
	if (surface)
		throw std::runtime_error("Surface::create() failed: surface already exist");

	if (pixels)
		surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, ((depth + 7) / 8) * width, format);
	else
		surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);

	if (!surface)
	{
		std::string message{"Surface::create() failed: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

void Surface::free()
{
	if (!surface)
		throw std::runtime_error("Surface::free() failed: surface is nullptr");
	SDL_FreeSurface(surface);
	surface = nullptr;
}

SDL_Surface* Surface::getPtr()
{
	if (!surface)
		throw std::runtime_error("Surface::getPtr() failed: surface is nullptr");
	return surface;
}

int Surface::getWidth()
{
	if (!surface)
		throw std::runtime_error("Surface::getWidth() failed: surface is nullptr");
	return surface->w;
}

int Surface::getHeight()
{
	if (!surface)
		throw std::runtime_error("Surface::getHeight() failed: surface is nullptr");
	return surface->h;
}

int Surface::getPitch()
{
	if (!surface)
		throw std::runtime_error("Surface::getPitch() failed: surface is nullptr");
	return surface->pitch;
}

void* Surface::getPixels()
{
	if (!surface)
		throw std::runtime_error("Surface::getPixels() failed: surface is nullptr");
	return surface->pixels;
}

SDL_PixelFormat* Surface::getFormat()
{
	if (!surface)
		throw std::runtime_error("Surface::getFormat() failed: surface is nullptr");
	return surface->format;
}

bool Surface::getMustLock()
{
	if (!surface)
		throw std::runtime_error("Surface::getMustLock() failed: surface is nullptr");
	return SDL_MUSTLOCK(surface);
}

Surface Surface::convert(const SDL_PixelFormat *fmt)
{
	if (!surface)
		throw std::runtime_error("Surface::convert() failed: surface is nullptr");

	SDL_Surface *temp{SDL_ConvertSurface(surface, fmt, 0)};
	if (!temp)
	{
		std::string message{"Surface::convert() failed: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}

	return temp;
}

void Surface::blitSurface(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect)
{
	if (!surface)
		throw std::runtime_error("Surface::blitSurface() failed: surface is nullptr");

	if (SDL_BlitSurface(surface, srcrect, dst.surface, dstrect) < 0)
	{
		std::string message("Surface::blitSurface() failed: ");
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

void Surface::blitScaled(Surface &dst, const SDL_Rect *srcrect, SDL_Rect *dstrect)
{
	if (!surface)
		throw std::runtime_error("Surface::blitScaled() failed: surface is nullptr");

	if (SDL_BlitScaled(surface, srcrect, dst.surface, dstrect) < 0)
	{
		std::string message("Surface::blitScaled() failed: ");
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

void Surface::lock()
{
	if (!surface)
		throw std::runtime_error("Surface::lock() failed: surface is nullptr");

	if (SDL_LockSurface(surface) < 0)
	{
		std::string message("Surface::lock() failed: ");
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

void Surface::unlock()
{
	if (!surface)
		throw std::runtime_error("Surface::unlock() failed: surface is nullptr");
	SDL_UnlockSurface(surface);
}

void Surface::setRLE(bool flag)
{
	if (!surface)
		throw std::runtime_error("Surface::setRLE() failed: surface is nullptr");

	if (SDL_SetSurfaceRLE(surface, flag) < 0)
	{
		std::string message{"Surface::setRLE() failed: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

Surface& Surface::operator=(Surface &&surface)
{
	if (this != &surface)
	{
		if (this->surface)
			free();
		this->surface = surface.surface;
		surface.surface = nullptr;
	}

	return *this;
}

