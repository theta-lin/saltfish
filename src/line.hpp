#ifndef LINE_HPP
#define LINE_HPP

#include <cmath>
#include "vec.hpp"
#include "color.hpp"
#include "memory.hpp"

class Line
{
public:
	Vec2d p0;
	Vec2d p1;

	Line() : p0{}, p1{}
	{
	}

	Line(const Vec2d &p0, const Vec2d &p1) : p0{p0}, p1{p1}
	{
	}

	void draw(const Color &color, FlatArray<Uint32> &canvas);
};

#endif // ifndef LINE_HPP

