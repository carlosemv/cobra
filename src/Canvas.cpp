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
	paint_pixel((height-1) - y, x, c);

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
		paint_pixel((height-1) - y, x, c);
	}
}

void Canvas::paint_pixel(unsigned x, unsigned y, Color c) 
{
	if (x < 0 or x >= height or y < 0 or y >= width)
		return;

	auto byte = data.get();
	byte += 3 * (width * x + y);

	auto fill_color = canvas_color(c);
	*byte++ = int(fill_color.r);
	*byte++ = int(fill_color.g);
	*byte = int(fill_color.b);
}

void Canvas::write_png(std::string file_name) const
{
	stbi_write_png(file_name.c_str(), width, height, 3, data.get(), 0);
}

Color Canvas::canvas_color(Color c)
{
	return (COLOR_DEPTH - 0.01F) * c;
}