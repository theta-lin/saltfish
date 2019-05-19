#ifndef CROSS_HPP
#define CROSS_HPP

#include <cassert>
#include <limits>

inline void checkCompatibility()
{
	assert(sizeof(double) == 8);
	assert(std::numeric_limits<double>::is_iec559);
	assert(std::numeric_limits<double>::has_infinity);
	assert(std::numeric_limits<double>::has_quiet_NaN);
}

#endif

