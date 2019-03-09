#ifndef __COLORS_H__
#define __COLORS_H__

#include <map>
#define GLM_FORCE_PURE // disable SIMD optimizations 
// based on compiler intrinsics, enable C++14 constexpr
#include <glm/glm.hpp>

using Color = glm::vec3;

class Colors
{
public:
	static constexpr auto BLACK = Color(0, 0, 0);
	static constexpr auto WHITE = Color(1, 1, 1);
	static constexpr auto MID_GRAY = Color(.5, .5, .5);
	static constexpr auto RED = Color(1, 0, 0);
	static constexpr auto GREEN = Color(0, 1, 0);
	static constexpr auto BLUE = Color(0, 0, 1);

	static const Color from_string(std::string name);
	void add_color(std::string name, Color value);

private:
	static std::map<std::string, Color> values;
};

#endif