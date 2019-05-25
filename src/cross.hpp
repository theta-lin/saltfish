#ifndef CROSS_HPP
#define CROSS_HPP

#include <limits>

inline void checkCompatibility()
{
	static_assert(sizeof(double) == 8, "Require size of double to be 8");
	static_assert(std::numeric_limits<double>::is_iec559, "Require IEEE 559 for double");
	static_assert(std::numeric_limits<double>::has_infinity, "Require infinity for double");
	static_assert(std::numeric_limits<double>::has_quiet_NaN, "Require quit NaN for double");
}

#endif // ifndef CROSS_HPP

