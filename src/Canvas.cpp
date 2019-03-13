#include "Canvas.h"

#include <iostream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Canvas::Canvas(unsigned _height, unsigned _width) :
	height(_height), width(_width), data_len(_height*_width*3)
{
	data = std::make_unique<data_t[]>(data_len);
}

void Canvas::fill(Color c) 
{
	auto byte = data.get();
	auto fill_color = canvas_color(c);
	for (unsigned i = 0; i < height; ++i) {
		for (unsigned j = 0; j < width; ++j) {
			*byte++ = int(fill_color.r);
			*byte++ = int(fill_color.g);
			*byte++ = int(fill_color.b);
		}
	}
}

void Canvas::paint_circle(Point center, unsigned radius, Color c) {
	int x = 0;
	int y = radius;

	int d = 1 - radius;
	auto dl = 3;
	auto dse = -2*y + 5;

	plot_circle_point(center, x, y, c);
	while (y > x) {
		if (d < 0) {
			d += dl;
			dse += 2;
		} else {
			d += dse;
			dse += 4;
			y--;
		}
		dl += 2;
		x++;
		plot_circle_point(center, x, y, c);
	}
}

void Canvas::plot_circle_point(Point center, int x, int y, Color c)
{
	paint_point(center.x + x, center.y + y, c);
	paint_point(center.x + x, center.y - y, c);
	paint_point(center.x - x, center.y + y, c);
	paint_point(center.x - x, center.y - y, c);

	paint_point(center.x + y, center.y + x, c);
	paint_point(center.x + y, center.y - x, c);
	paint_point(center.x - y, center.y + x, c);
	paint_point(center.x - y, center.y - x, c);
}

void Canvas::paint_point(int x, int y, Color c)
{
	paint_pixel((height-1) - y, x, c);
}

void Canvas::paint_line(Point start, Point end, Color c)
{
	if (start.x > end.x)
		std::swap(start, end);

	auto dx = end.x - start.x;
	auto dy = end.y - start.y;

	auto x_inc = dx;
	auto y_inc = dy;

	if (x_inc != 0)
		x_inc = (x_inc > 0) ? 1 : -1;
	if (y_inc != 0)
		y_inc = (y_inc > 0) ? 1 : -1;

	dx = std::abs(dx);
	dy = std::abs(dy);

	bool swapped = false;
	if (dy > dx) {
		std::swap(dx, dy);
		swapped = true;
	}

	auto pk = 2 * dy - dx;
	auto i0 = 2 * dy;
	auto i1 = i0 - 2 * dx;

	auto x = start.x;
	auto y = start.y;
	paint_point(x, y, c);

	for (auto p = 1; p <= dx; ++p) {
		if (pk < 0) {
			if (swapped)
				y += y_inc;
			else
				x += x_inc;
			pk += i0;
		} else {
			y += y_inc;
			x += x_inc;
			pk += i1;
		}
		paint_point(x, y, c);
	}
}

void Canvas::flood_fill(Point flood, Color fill, Color old)
{
	if (fill == old)
		return;
	flood_fill((height-1) - flood.y, flood.x, fill, old);
}

void Canvas::flood_fill(int x, int y, Color fill, Color old)
{
	std::queue<ipair_t> pixels;
	pixels.push({x, y});

	while (not pixels.empty()) {
		auto [x, y] = pixels.front();

		if (valid_pixel(x, y) and pixel_is_color(x, y, old)) {
			paint_pixel(x, y, fill);

			pixels.push({x+1, y});
			pixels.push({x, y+1});
			pixels.push({x-1, y});
			pixels.push({x, y-1});
		}

		pixels.pop();
	}
}

bool Canvas::valid_pixel(int x, int y) const
{
	return (x >= 0 and x < (int)height and y >= 0 and y < (int)width);
}

void Canvas::paint_pixel(unsigned x, unsigned y, Color c) 
{
	if (not valid_pixel(x, y))
		return;

	auto byte = data.get();
	byte += 3 * (width * x + y);

	auto fill_color = canvas_color(c);
	*byte++ = int(fill_color.r);
	*byte++ = int(fill_color.g);
	*byte = int(fill_color.b);
}

bool Canvas::pixel_is_color(int x, int y, Color c) const
{
	if (not valid_pixel(x, y))
		return false;

	auto byte = data.get();
	byte += 3 * (width * x + y);

	auto fill_color = canvas_color(c);
	return (*byte++ == int(fill_color.r)
		and *byte++ == int(fill_color.g)
		and *byte == int(fill_color.b));
}

void Canvas::write_png(std::string file_name) const
{
	stbi_write_png(file_name.c_str(), width, height, 3, data.get(), 0);
}

Color Canvas::canvas_color(Color c)
{
	return (COLOR_DEPTH - 0.01F) * c;
}