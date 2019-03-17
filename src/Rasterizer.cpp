#include "Rasterizer.h"

void Rasterizer::rasterize()
{
	this->canvas = Canvas(scene.height, scene.width);
	canvas.fill(scene.bg_color);

	for (auto obj : scene.objects) {
		if (obj.is_polygon() and obj.fill) {
			if (obj.fill == FillMethod::Scanline) {
				canvas.scanline_fill(obj.edges.value(),
					obj.fill_color.value());
			}
		}

		switch (obj.type) {
			case ObjectType::Line:
			{
				canvas.paint_line(obj.points[0], obj.points[1],
					obj.line_color);
				break;
			}
			case ObjectType::Polyline:
			case ObjectType::Polygon:
			case ObjectType::Rect:
			{
				auto point = obj.points.begin();
				for (; point+1 != obj.points.end(); ++point) {
					canvas.paint_line(*point, *(point+1), obj.line_color);
				}
				break;
			}
			case ObjectType::Circle:
			{
				canvas.paint_circle(obj.points[0], obj.radius.value(),
					obj.line_color);
				break;
			}
			case ObjectType::Arc:
			{
				canvas.paint_arc(obj.points[0], obj.radius.value(),
					obj.line_color, obj.arc.value());
				break;
			}
			default:
				std::cerr << obj.type_name << " not rasterized\n";
		}

		if (obj.is_closed() and obj.fill) {
			if (obj.fill == FillMethod::Flood) {
				for (auto p : *obj.flood_points) {
					canvas.flood_fill(p, obj.fill_color.value());
				}
			}
		}
	}

	if (antialiasing) {
		std::vector<int> gaussian = {1, 2, 1, 2, 4, 2, 1, 2, 1};
		canvas.antialias(gaussian, 3);
	}
}

void Rasterizer::load_conf(std::string _f)
{
	YAML::Node config = YAML::LoadFile(_f);
	if (config["antialiasing"])
		this->antialiasing = config["antialiasing"].as<bool>();
	else
		this->antialiasing = true;
}
