#include "surface.hpp"

PixelView::PixelView(Uint8 *const pixel, const SDL_PixelFormat *const format) : pixel{pixel}, format{format}
{
}

PixelView::operator Uint32() const
{
	switch (format->BytesPerPixel)
	{
	case 1:
		return *pixel;
	case 2:
		return *reinterpret_cast<Uint16*>(pixel);
	case 3:
		{
			Uint32 result{  (static_cast<Uint32>(pixel[0]))
						  + (static_cast<Uint32>(pixel[1]) << 8)
						  + (static_cast<Uint32>(pixel[2]) << 16)};
			return result;
		}
	case 4:
		return *reinterpret_cast<Uint32*>(pixel);
	default:
		throw std::runtime_error("PixelView::operator Uint32() failed: pixel byte size error");
	}
}

PixelView::operator SDL_Color() const
{
	SDL_Color color;
	SDL_GetRGBA(static_cast<Uint32>(*this), format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

PixelView& PixelView::operator=(const SDL_Color &color)
{
	Uint32 value{SDL_MapRGBA(format, color.r, color.g, color.b, color.a)};
	switch (format->BytesPerPixel)
	{
	case 1:
		*pixel = static_cast<Uint8>(value);
		break;
	case 2:
		*reinterpret_cast<Uint16 *>(pixel) = static_cast<Uint16>(value);
		break;
	case 3:
		pixel[0] = static_cast<Uint8>(value);
		pixel[1] = static_cast<Uint8>(value << 8);
		pixel[2] = static_cast<Uint8>(value << 16);
		break;
	case 4:
		*reinterpret_cast<Uint32 *>(pixel) = value;
		break;
	default:
		throw std::runtime_error("PixelView::operator= failed: pixel byte size error");
	}

	return *this;
}

PixelView& PixelView::operator=(const PixelView &pixelView)
{
	SDL_Color color{static_cast<SDL_Color>(pixelView)};
	*this = color;
	return *this;
}


Surface::Surface(SDL_Surface *surface) : surface{surface}
{
}

Surface::Surface(Surface &&surface) : surface{surface.getPtr()}
{
	if (surface.getPtr())
		surface.free();
}

Surface::Surface(int width, int height, int depth, Uint32 format) : surface{nullptr}
{
	create(width, height, depth, format);
}

Surface::Surface(void *pixels, int width, int height, int depth, int pitch, Uint32 format) : surface{nullptr}
{
	create(pixels, width, height, depth, pitch, format);
}

Surface::~Surface()
{
	if (surface)
		free();
}

void Surface::create(int width, int height, int depth, Uint32 format)
{
	if (surface)
		throw std::runtime_error("Surface::create() failed: surface already exist");

	surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, depth, format);
	if (!surface)
	{
		std::string message{"Surface::create() failed: "};
		message.append(SDL_GetError());
		throw std::runtime_error(message);
	}
}

void Surface::create(void *pixels, int width, int height, int depth, int pitch, Uint32 format)
{
	if (surface)
		throw std::runtime_error("Surface::create() failed: surface already exist");

	surface = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, depth, pitch, format);
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

void Surface::fillRect(const SDL_Rect *rect, const SDL_Color &color)
{
	if (!surface)
		throw std::runtime_error("Surface::fillRect() failed: surface is nullptr");

	if (SDL_FillRect(surface, rect, SDL_MapRGBA(getFormat(), color.r, color.g, color.b, color.a)) < 0)
	{
		std::string message("Surface::fillRect() failed: ");
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

