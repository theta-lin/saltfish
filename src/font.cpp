#include "font.hpp"

namespace sw
{

Font::Font() : font{nullptr}
{
}

Font::Font(const std::filesystem::path &file, int ptsize, long index) : font{nullptr}
{
	open(file, ptsize, index);
}

Font::~Font()
{
	if (font)
		close();
}

void Font::open(const std::filesystem::path &file, int ptsize, long index)
{
	if (font)
		throw std::runtime_error{"Font::open() failed: font already open"};

	font = TTF_OpenFontIndex(file.c_str(), ptsize, index);
	if (!font)
	{
		std::string message{"Font::open() failed: "};
		message += TTF_GetError();
		throw std::runtime_error{message};
	}
}

void Font::close()
{
	if (!font)
		throw std::runtime_error{"Font::close() failed: font is nullptr"};

	TTF_CloseFont(font);
	font = nullptr;
}

Font::operator bool()
{
	return font != nullptr;
}

Surface Font::renderSolid(std::string_view text, const Color &fg)
{
	Surface surface{TTF_RenderText_Solid(font, text.data(), fg)};
	if (!surface)
	{
		std::string message{"Font::renderSolid() failed: "};
		message += TTF_GetError();
		throw std::runtime_error{message};
	}
	return surface;
}

Surface Font::renderShaded(std::string_view text, const ColorPair &color)
{
	Surface surface{TTF_RenderText_Shaded(font, text.data(), color.first, color.second)};
	if (!surface)
	{
		std::string message{"Font::renderShaded() failed: "};
		message += TTF_GetError();
		throw std::runtime_error{message};
	}
	return surface;
}

Surface Font::renderBlended(std::string_view text, const Color &fg)
{
	Surface surface{TTF_RenderText_Blended(font, text.data(), fg)};
	if (!surface)
	{
		std::string message{"Font::renderBlended() failed: "};
		message += TTF_GetError();
		throw std::runtime_error{message};
	}
	return surface;
}

void Font::setStyle(int style)
{
	TTF_SetFontStyle(font, style);
}

int Font::getStyle()
{
	return TTF_GetFontStyle(font);
}

void Font::setOutline(int outline)
{
	TTF_SetFontOutline(font, outline);
}

int Font::getOutline()
{
	return TTF_GetFontOutline(font);
}

long Font::fontFaces()
{
	return TTF_FontFaces(font);
}

int Font::fontFaceIsFixedWidth()
{
	return TTF_FontFaceIsFixedWidth(font);
}

std::string Font::fontFaceFamilyName()
{
	return TTF_FontFaceFamilyName(font);
}

std::string Font::fontFaceStyleName()
{
	return TTF_FontFaceStyleName(font);
}

} // namespace sw

