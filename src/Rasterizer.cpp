#include "Rasterizer.h"

void Rasterizer::rasterize()
{
	this->canvas = Canvas(scene.height, scene.width);
	canvas.fill(scene.bg_color);

	for (auto obj : scene.objects) {
		switch (obj.type) {
			case ObjectType::Line:
				canvas.paint_line(obj.points[0], obj.points[1], obj.line_color);
				break;
			default:
				std::cout << obj.type_name << " not rasterized\n";
		}
	}
}