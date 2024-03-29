#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <memory>
#include <string>
#include <algorithm>
#include <functional>
#include <limits>
#include <queue>
#include <list>

#define GLM_FORCE_PURE // disable SIMD optimizations 
// based on compiler intrinsics, enable C++14 constexpr
#include <glm/glm.hpp>

#include "Colors.h"
#include "Point.h"
#include "Edge.h"
#include "Arc.h"

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
	void paint_arc(Point center, unsigned radius, Color c, Arc arc);
	void scanline_fill(std::list<Edge> edges, Color c);
	void flood_fill(Point flood, Color fill);
	void write_png(std::string file_name) const;
	void antialias(std::vector<int> kernel, unsigned dim);

private:
	unsigned height, width;
	std::unique_ptr<data_t[]> data;
	unsigned data_len;

	bool valid_pixel(int x, int y) const;
	void flood_fill(int x, int y, Color fill, Color old);
	Color get_color(int x, int y) const;
	bool pixel_is_color(int x, int y, Color c) const;
	void plot_arc(Point center, unsigned radius, Color c, Arc arc,
		std::function<void(Point, int, int, Color, Arc)> plot_func);
	void plot_circle_point(Point center, int x, int y, Color c, Arc arc);
	void plot_arc_point(Point center, int x, int y, Color c, Arc arc);
	void paint_scanline(int y, int x0, int x1, Color c);
	void paint_point(int x, int y, Color c);
	void paint_pixel(unsigned x, unsigned y, Color c);

	static Color canvas_color(Color c);
	static Color normal_color(Color c);
	static bool valid_color(Color c);

	static constexpr double PI = 3.141592653589;
	static constexpr double TAU = 2 * PI;
};

#endif
