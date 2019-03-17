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

	for (auto point : config["points"]) {
		auto name = point[0].as<std::string>();
		auto value = point[1].as<ipair_t>();
		points[name] = value;
	}

	if (config["background"])
		this->bg_color = get_color(config["background"]);
	else
		this->bg_color = Scene::DEFAULT_BG;

	for (auto node : config["objects"]) {
		auto type_name = node["type"].as<std::string>();
		ObjectType type;
		try {
			type = Object::type_names.at(type_name);
		} catch (std::out_of_range) {
			std::cerr << "Invalid object type " << type_name
				<< "; ignoring object.\n";
			continue;
		}

		Object obj;
		switch (type) {
			case ObjectType::Line:
			{
				auto start = get_point(node["start"]);
				auto end = get_point(node["end"]);
				obj = Object(type_name, {start, end});
				break;
			}
			case ObjectType::Polyline:
			{
				std::vector<Point> points;
				for (auto p : node["points"])
					points.push_back(get_point(p));
				obj = Object(type_name, points);
				break;
			}
			case ObjectType::Polygon:
			{
				std::vector<Point> points;
				for (auto p : node["points"])
					points.push_back(get_point(p));
				points.push_back(points.front());
				obj = Object(type_name, points);
				break;
			}
			case ObjectType::Circle:
			{
				auto center = get_point(node["center"]);
				auto radius = node["radius"].as<int>();
				obj = Object(type_name, {center});
				obj.radius = radius;
				break;
			}
			case ObjectType::Arc:
			{
				auto center = get_point(node["center"]);
				auto arc = get_arc(node["center"]);
				auto radius = node["radius"].as<int>();
				obj = Object(type_name, {center});
				obj.radius = radius;
				obj.arc = arc;
				break;
			}
			case ObjectType::Rect:
			{
				auto c = get_point(node["corner"]);
				auto width = node["width"].as<int>();
				auto height = node["height"].as<int>();

				std::vector<Point> points = {
					{c.x, c.y},
					{c.x+width, c.y},
					{c.x+width, c.y-height},
					{c.x, c.y-height},
					{c.x, c.y}
				};
				obj = Object(type_name, points);

				break;
			}
			default:
				std::cerr << "unexpected type " <<
					type_name << std::endl;
		}

		if (obj.is_polygon()) {
			obj.edges = std::list<Edge>();
			auto point = obj.points.begin();
			for (; point+1 != obj.points.end(); ++point) {
				Edge e(*point, *(point+1));
				obj.edges.value().push_back(e);
			}
		}

		if (node["color"])
			obj.line_color = get_color(node["color"]);
		else
			obj.line_color = Scene::DEFAULT_LINE;

		if (node["fill"] or node["fill_color"] or node["flood_points"]) {
			if (node["fill_color"])
				obj.fill_color = get_color(node["fill_color"]);
			else
				obj.fill_color = Scene::DEFAULT_FILL;

			if (node["fill"]) {
				auto fill_n = node["fill"].as<std::string>();
				obj.fill = Object::fill_names.at(fill_n);
			} else if (node["flood_points"]) {
				obj.fill = FillMethod::Flood;
			} else {
				obj.fill = FillMethod::Scanline;
			}

			if (obj.fill == FillMethod::Flood) {
				std::vector<Point> points;
				for (auto p : node["flood_points"])
					points.push_back(get_point(p));
				obj.flood_points = points;
			}
		}

		if (node["name"]) {
			obj.name = node["name"].as<std::string>();
		}

		this->objects.push_back(obj);
	}
}

Point Scene::get_point(YAML::Node node) const
{
	try {
		auto value = node.as<ipair_t>();
		return value;
	} catch (const YAML::BadConversion& e) {
		auto name = node.as<std::string>();
		return points.at(name);
	}
}

Point Scene::get_arc(YAML::Node node) const
{
	Arc arc;
	try {
		arc = Arc(node.as<ipair_t>());
	} catch (const YAML::BadConversion& e) {
		auto name = node.as<std::string>();
		arc = arcs.at(name);
	}

	if (arc.x < 0 or arc.x > 1 or arc.y < 0 or arc.y > 1) {
		std::cerr << "Invalid arc " << arc.x << ", " << arc.y;
		arc = Arc(0, 1);
		std::cerr << "; using " << arc.x << ", " << arc.y
			<< " instead\n";
	}

	return arc;
}

Color Scene::get_color(YAML::Node node) const
{
	try {
		auto value = node.as<std::array<double, 3>>();
		return {value[0], value[1], value[2]};
	} catch (const YAML::BadConversion& e) {
		auto name = node.as<std::string>();
		return palette.from_string(name);
	}
}