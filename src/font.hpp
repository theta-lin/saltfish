#ifndef FONT_HPP
#define FONT_HPP

#include "surface.hpp"
#include <SDL_ttf.h>
#include <filesystem>

namespace sw // Sdl Wrapper
{

/*
 * A simple wrapper for TTF_Font
 * NOTE: Due to the specification of SDL, const object will be unavailable
 */
class Font
{
private:
	TTF_Font *font;

public:
	Font();
	Font(Font &font) = delete;
	Font(const std::filesystem::path &file, int ptsize, long index = 0);
	~Font();

	void open(const std::filesystem::path &file, int ptsize, long index = 0);
	void close();
	operator bool();

	// Always use utf-8 text when rendering font
	Surface renderSolid(std::string_view text, const Color &fg);
	Surface renderShaded(std::string_view text, const ColorPair &color);
	Surface renderBlended(std::string_view text, const Color &fg);

	void setStyle(int style);
	int getStyle();
	void setOutline(int outline);
	int getOutline();

	long fontFaces();
	int fontFaceIsFixedWidth();
	std::string fontFaceFamilyName();
	std::string fontFaceStyleName();
};

} // namespace sw

#endif // ifndef FONT_HPP

