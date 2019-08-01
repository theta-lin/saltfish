#include "line.hpp"

void Line::draw(const sw::Color &color, sw::Surface &surface)
{
	double x0{p0[0]}, x1{p1[0]};
	double y0{p0[1]}, y1{p1[1]};
	double dX{x1 - x0}, dY{y1 - y0};

	bool steep{dY > dX};
	if (steep)
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		std::swap(dX, dY);
	}

	int signX{dX > 0 ? 1 : -1}, signY{dY > 0 ? 1: -1};
	double dError{std::abs(dY / dX)};
	double error{0.0};
	for (int x{static_cast<int>(std::lround(x0))}, y{static_cast<int>(std::lround(y0))}; x <= std::lround(x1); x += signX)
	{
		if (steep)
		{
			surface(y, x) = color;
		}
		else
		{
			surface(x, y) = color;
		}

		error += dError;
		if (error >= 0.5)
		{
			y += signY;
			error -= 1.0;
		}
	}
}

