#ifndef LINE_SHAPE_HPP
#define LINE_SHAPE_HPP

#include "surface.hpp"
#include "vec.hpp"

/*
 * The 2D line primative using Bresenham's LineShape Algorithm
 */
class LineShape
{
public:
	Vec2d p0;
	Vec2d p1;

	LineShape() : p0{}, p1{}
	{
	}

	LineShape(const Vec2d &p0, const Vec2d &p1) : p0{p0}, p1{p1}
	{
	}

	void draw(const sw::Color &color, sw::Surface &surface);
};

#endif // ifndef LINE_SHAPE_HPP

