#ifndef LINE_HPP
#define LINE_HPP

#include <cmath>
#include "vec.hpp"
#include "surface.hpp"

/*
 * The 2D line primative using Bresenham's Line Algorithm
 */
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

	void draw(const sw::Color &color, sw::Surface &surface);
};

#endif // ifndef LINE_HPP

