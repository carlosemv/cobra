#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <memory>
#include <string>
#include <algorithm>
#include <limits>

#define GLM_FORCE_PURE // disable SIMD optimizations 
// based on compiler intrinsics, enable C++14 constexpr
#include <glm/glm.hpp>

#include "Colors.h"
#include "Point.h"

using data_t = unsigned char;

class Canvas
{
public:
	static constexpr unsigned COLOR_DEPTH = 256;
	static_assert(COLOR_DEPTH-1 <= 
		std::numeric_limits<data_t>::max());
	
	Canvas() : height(0), width(0) {}
	Canvas(unsigned _height, unsigned _width);
	void fill(Color c);
	void paint_line(Point start, Point end, Color c);
	void paint_circle(Point center, unsigned radius, Color c);
	void write_png(std::string file_name) const;

private:
	unsigned height, width;
	std::unique_ptr<data_t[]> data;
	unsigned data_len;

	void plot_circle_point(Point center, int x, int y, Color c);
	void paint_point(int x, int y, Color c);
	void paint_pixel(unsigned x, unsigned y, Color c);

	static Color canvas_color(Color c);
};

#endif