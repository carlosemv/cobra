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

void Canvas::paint_arc(Point center, unsigned radius, Color c, Arc arc)
{
	plot_arc(center, radius, c, arc, 
		std::bind(&Canvas::plot_arc_point, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
			std::placeholders::_5));
}

void Canvas::paint_circle(Point center, unsigned radius, Color c)
{
	Arc arc = {0, 1};
	plot_arc(center, radius, c, arc, 
		std::bind(&Canvas::plot_circle_point, this,
			std::placeholders::_1,
			std::placeholders::_2,
			std::placeholders::_3,
			std::placeholders::_4,
			std::placeholders::_5));
}

void Canvas::plot_arc(Point center, unsigned radius, Color c, Arc arc,
	std::function<void(Point, int, int, Color, Arc)> plot_func)
{
	int x = 0;
	int y = radius;

	int d = 1 - radius;
	auto dl = 3;
	auto dse = -2*y + 5;

	plot_func(center, x, y, c, arc);
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
		plot_func(center, x, y, c, arc);
	}
}

void Canvas::plot_circle_point(Point center, int x, int y, Color c, [[maybe_unused]] Arc arc)
{
	paint_point(center.x + x, center.y + y, c);
	paint_point(center.x + y, center.y + x, c);
	paint_point(center.x + y, center.y - x, c);
	paint_point(center.x + x, center.y - y, c);
	
	paint_point(center.x - x, center.y - y, c);
	paint_point(center.x - y, center.y - x, c);
	paint_point(center.x - y, center.y + x, c);
	paint_point(center.x - x, center.y + y, c);
}

void Canvas::plot_arc_point(Point center, int x, int y, Color c, Arc arc)
{
	for (auto xs : {-1, 1}) {
		for (auto ys: {-1, 1}) {
			for (auto flip : {true, false}) {
				auto i = center.x;
				auto j = center.y;

				if (flip) {
					i += xs * y;
					j += ys * x;
				} else {
					i += xs * x;
					j += ys * y;
				}

				auto rad = std::atan2(j - center.y, i - center.x);
				rad = (rad + Canvas::PI) / Canvas::TAU;
				rad = std::fmod(rad + .5, 1.0);

				if (arc.x <= arc.y) {
					if (rad >= arc.x and rad <= arc.y) {
						paint_point(i, j, c);
					}
				} else {
					if ((rad >= arc.x and rad <= 1)
						or (rad >= 0 and rad <= arc.y)) {
						paint_point(i, j, c);
					}
				}
			}
		}
	}
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

void Canvas::flood_fill(Point flood, Color fill)
{
	auto flood_pixel = Point((height-1) - flood.y, flood.x);
	Color old = normal_color(get_color(flood_pixel.x, flood_pixel.y));
	if (fill == old)
		return;
	flood_fill(flood_pixel.x, flood_pixel.y, fill, old);
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

void Canvas::scanline_fill(std::list<Edge> edges, Color c)
{
	edges.sort(Edge::et_compare);

	auto y = edges.front().y_min;
	std::list<Edge> aet;
	while (not aet.empty() or not edges.empty()) {
		while (not edges.empty() and edges.front().y_min == y) {
			if (edges.front().inv_m.second != 0)
				aet.push_back(edges.front());
			edges.pop_front();
		}

		aet.remove_if([y](Edge e){ return e.y_max == y; });

		aet.sort([](Edge& e1, Edge& e2){ return e1.x_min < e2.x_min; });

		if (aet.size() % 2 == 0) {
			for (auto e = aet.begin(); e != aet.end(); std::advance(e, 2))
				paint_scanline(y, e->x_min, std::next(e)->x_min, c);
		}

		y++;
		for (auto& e : aet) {
			e.x_min += (e.inv_m.first + e.x_remain) / e.inv_m.second;
			e.x_remain += e.inv_m.first % e.inv_m.second;
			e.x_remain %= e.inv_m.second;
		}
	}
}

void Canvas::paint_scanline(int y, int x0, int x1, Color c)
{
	for (auto i = x0+1; i < x1; ++i) {
		paint_point(i, y, c);
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

Color Canvas::get_color(int x, int y) const
{
	if (not valid_pixel(x, y))
		return {0, 0, 0};

	auto byte = data.get();
	byte += 3 * (width * x + y);

	Color c;
	c.r = *byte++;
	c.g = *byte++;
	c.b = *byte;
	return c;
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
	return (COLOR_DEPTH - 1.F) * c;
}

Color Canvas::normal_color(Color c)
{
	return c / (COLOR_DEPTH - 1.F);
}

bool Canvas::valid_color(Color c)
{
	return (c.r >= 0 and c.r <= 1 
		and c.g >= 0 and c.g <= 1
		and c.b >= 0 and c.b <= 1);
}

void Canvas::antialias(std::vector<int> kernel, unsigned dim)
{
	if (kernel.size() != dim*dim or dim % 2 == 0) {
		std::cerr << "Invalid dimension " << dim;
		std::cerr << " for provided kernel\n";
		return;
	}

	std::vector<Point> offsets;
	for (unsigned p = 0; p < dim*dim; ++p) {
		auto i = p / dim;
		auto j = p % dim;

		auto diff = Point(i, j) - Point(dim/2, dim/2);
		offsets.push_back({diff.x, diff.y});
	}

	auto new_canvas = std::make_unique<data_t[]>(data_len);
	std::copy(data.get(), data.get()+data_len, new_canvas.get());
	auto byte = new_canvas.get();
	for (unsigned i = 0; i < height; ++i) {
		for (unsigned j = 0; j < width; ++j) {
			float normal = 0;
			Color c = {0, 0, 0};
			for (size_t p = 0; p < kernel.size(); ++p) {
				auto curr_point = Point(i, j) + offsets[p];
				if (valid_pixel(curr_point.x, curr_point.y)) {
					normal += kernel[p];
					auto curr_color = normal_color(get_color(curr_point.x, curr_point.y));
					c += curr_color * float(kernel[p]);
				}
			}

			c /= normal;
			if (valid_color(c)) {
				auto fill_color = canvas_color(c);
				*byte++ = int(fill_color.r);
				*byte++ = int(fill_color.g);
				*byte++ = int(fill_color.b);
			} else {
				byte += 3;
			}
		}
	}

	std::copy(new_canvas.get(), new_canvas.get()+data_len, data.get());
}

