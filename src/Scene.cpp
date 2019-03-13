#include "Scene.h"

Scene::Scene(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	this->height = config["height"].as<int>();
	this->width = config["width"].as<int>();

	for (auto color : config["colors"]) {
		auto name = color["name"].as<std::string>();
		auto value = color["rgb"].as<std::array<double, 3>>();

		palette.add_color(name, {value[0], value[1], value[2]});
	}

	if (config["background"]) {
		auto bg = config["background"].as<std::string>();
		this->bg_color = palette.from_string(bg);
	} else {
		this->bg_color = Scene::DEFAULT_BG;
	}

	for (auto node : config["objects"]) {
		auto type_name = node["type"].as<std::string>();
		auto type = Object::type_names.at(type_name);

		Object obj;
		switch (type) {
			case ObjectType::Line:
			{
				auto start = node["start"].as<ipair_t>();
				auto end = node["end"].as<ipair_t>();
				obj = Object(type_name, {start, end});
				break;
			}
			case ObjectType::Polyline:
			{
				std::vector<Point> points;
				for (auto p : node["points"])
					points.push_back(p.as<ipair_t>());
				obj = Object(type_name, points);
				break;
			}
			case ObjectType::Polygon:
			{
				std::vector<Point> points;
				for (auto p : node["points"])
					points.push_back(p.as<ipair_t>());
				points.push_back(points.front());
				obj = Object(type_name, points);
				break;
			}
			case ObjectType::Circle:
			{
				auto center = node["center"].as<ipair_t>();
				auto radius = node["radius"].as<int>();
				obj = Object(type_name, {center});
				obj.radius = radius;
				break;
			}
			default:
				std::cout << "unexpected type!\n"; break;
		}

		std::cout << obj.type_name << std::endl;

		if (node["color"]) {
			auto lcolor = node["color"].as<std::string>();
			obj.line_color = palette.from_string(lcolor);
		} else {
			obj.line_color = Scene::DEFAULT_LINE;
		}

		if (node["fill"] or node["fill_color"] or node["flood_points"]) {
			if (node["fill_color"]) {
				auto fcolor = node["fill_color"].as<std::string>();
				obj.fill_color = palette.from_string(fcolor);
			} else {
				obj.fill_color = Scene::DEFAULT_FILL;
			}

			if (node["fill"]) {
				auto fill_n = node["fill"].as<std::string>();
				obj.fill = Object::fill_names.at(fill_n);
			} else if (node["flood_points"]) {
				obj.fill = FillMethod::Flood;
			} else {
				obj.fill = FillMethod::Scanline;
			}

			if (obj.fill == FillMethod::Flood) {
				if (node["flood_points"][0].IsSequence()) {
					std::vector<Point> points;
					for (auto p : node["flood_points"])
						points.push_back(p.as<ipair_t>());
					obj.flood_points = points;
				} else {
					auto point = node["flood_points"].as<ipair_t>();
					obj.flood_points = {point};
				}
			}
		}

		if (node["name"]) {
			obj.name = node["name"].as<std::string>();
		}

		this->objects.push_back(obj);
	}
}