#ifndef __ARC_H__
#define __ARC_H__

#define GLM_FORCE_PURE // disable SIMD optimizations 
// based on compiler intrinsics, enable C++14 constexpr
#include <glm/glm.hpp>
#include <iostream>

using dpair_t = std::pair<double, double>;

struct Arc : glm::dvec2
{
	// inherit all constructors
	using glm::dvec2::dvec2;
	Arc(dpair_t _a) : glm::dvec2(_a.first, _a.second) {}

	inline friend std::ostream& operator<<(std::ostream& out, const Arc& a)
	{
		return out << "[" << a.x << ", " << a.y << "]";
	}
};

#endif