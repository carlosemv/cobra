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

void Canvas::paint_line(Point start, Point end, Color c) {
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