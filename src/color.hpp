#ifndef COLOR_HPP
#define COLOR_HPP

#include "SDL.h"

struct Color
{
public:
	Uint8 r;
	Uint8 g;
	Uint8 b;
	Uint8 a;

	Color() : r{0}, g{0}, b{0}, a{0}
	{
	}

	Color(Uint8 r, Uint8 g, Uint8 b, Uint8 a) : r{r}, g{g}, b{b}, a{a}
	{
	}

	operator Uint32() const
	{
		return  (static_cast<Uint32>(r) << 24)
			  + (static_cast<Uint32>(g) << 16)
			  + (static_cast<Uint32>(b) << 8 )
			  + (static_cast<Uint32>(a)      );
	}
};

#endif

