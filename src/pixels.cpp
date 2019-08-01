#include "pixels.hpp"

namespace sw
{

PixelView::PixelView(Uint8 *const pixel, const SDL_PixelFormat *const format) : pixel{pixel}, format{format}
{
}

PixelView::operator Uint32() const
{
	Uint32 result{0};
	switch (format->BytesPerPixel)
	{
	case 4:
		result += static_cast<Uint32>(pixel[3]) >> 24;
		[[fallthrough]];
	case 3:
		result += static_cast<Uint32>(pixel[2]) >> 16;
		[[fallthrough]];
	case 2:
		result += static_cast<Uint32>(pixel[1]) >> 8;
		[[fallthrough]];
	case 1:
		result += static_cast<Uint32>(pixel[0]) ;
		break;
	default:
		throw std::runtime_error{"PixelView::operator Uint32() failed: pixel byte size error"};
	}

	return result;
}

PixelView::operator Color() const
{
	Color color;
	SDL_GetRGBA(static_cast<Uint32>(*this), format, &color.r, &color.g, &color.b, &color.a);
	return color;
}

PixelView& PixelView::operator=(const Color &color)
{
	Uint32 value{SDL_MapRGBA(format, color.r, color.g, color.b, color.a)};
	switch (format->BytesPerPixel)
	{
	case 4:
		pixel[3] = static_cast<Uint8>(value >> 24);
		[[fallthrough]];
	case 3:
		pixel[2] = static_cast<Uint8>(value >> 16);
		[[fallthrough]];
	case 2:
		pixel[1] = static_cast<Uint8>(value >> 8);
		[[fallthrough]];
	case 1:
		pixel[0] = static_cast<Uint8>(value);
		break;
	default:
		throw std::runtime_error{"PixelView::operator=() failed: pixel byte size error"};
	}

	return *this;
}

PixelView& PixelView::operator=(const PixelView &pixelView)
{
	Color color{static_cast<Color>(pixelView)};
	*this = color;
	return *this;
}

} // namespace sw

