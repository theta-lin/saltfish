/*
 * This file defines wrapper to pixel-level data and function from SDL
 */

#ifndef PIXELS_HPP
#define PIXELS_HPP

#include <stdexcept>
#include <utility>
#include <SDL.h>

namespace sw // Sdl Wrapper
{

/*
 * Unify the code style
 */
using Rect = SDL_Rect;
using Color = SDL_Color;
using ColorPair = std::pair<sw::Color, sw::Color>;

/*
 * A wrapper(reference) for a pixel on SDL_Surface
 */
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
	operator Color() const;
	PixelView& operator=(const Color &color);
	PixelView& operator=(const PixelView &pixelView);
};

} // namespace sw

#endif // PIXELS_HPP

