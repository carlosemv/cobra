#ifndef __POINT_H__
#define __POINT_H__

#define GLM_FORCE_PURE // disable SIMD optimizations 
// based on compiler intrinsics, enable C++14 constexpr
#include <glm/glm.hpp>
#include <iostream>

using ipair_t = std::pair<int, int>;

struct Point : glm::ivec2
{
	// inherit all constructors
	using glm::ivec2::ivec2;
	Point(ipair_t _p) : glm::ivec2(_p.first, _p.second) {}

	inline friend std::ostream& operator<<(std::ostream& out, const Point& p)
	{
		return out << "[" << p.x << ", " << p.y << "]";
	}
};

using Arc = Point;

#endif