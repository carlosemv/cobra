#include "Scene.h"

Scene::Scene(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	height = config["height"].as<int>();
	width = config["width"].as<int>();

	load_collections(config);

	if (config["background"])
		bg_color = get_color(config["background"]);
	else
		bg_color = Scene::DEFAULT_BG;

	for (auto node : config["objects"]) {
		std::string type_name;
		auto o_type_name = get_value<std::string>(node, "type");
		if (o_type_name) {
			type_name = o_type_name.value();
		} else {
			std::cerr << "Missing object type; ignoring object.\n";
			continue;
		}

		auto obj_name = get_value<std::string>(node, "name");

		Object obj;
		auto o_obj = make_object(node, type_name, obj_name);
		if (o_obj) {
			obj = o_obj.value();
		} else {
			continue;
		}

		if (node["name"]) {
			obj.name = node["name"].as<std::string>();
		}

		if (obj.is_polygon()) {
			obj.edges = std::list<Edge>();
			auto point = obj.points.begin();
			for (; point+1 != obj.points.end(); ++point) {
				Edge e(*point, *(point+1));
				obj.edges.value().push_back(e);
			}
		}

		if (node["color"]) {
			obj.line_color = get_color(node["color"]);
		} else {
			obj.line_color = Scene::DEFAULT_LINE;
		}

		load_fill(node, obj);

		this->objects.push_back(obj);
	}
}


std::optional<Object> Scene::make_object(YAML::Node& node, std::string type_name,
	std::optional<std::string> obj_name) const
{
	ObjectType type;
	try {
		type = Object::type_names.at(type_name);
	} catch (const std::out_of_range& e) {
		std::cerr << "Invalid object type " << type_name
			<< "; ignoring object.\n";
		return std::nullopt;
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
			auto circle = make_circle(node, type_name, obj_name);
			if (circle)
				obj = circle.value();
			else
				return std::nullopt;
			break;
		}
		case ObjectType::Arc:
		{
			auto arc = make_arc(node, type_name, obj_name);
			if (arc)
				obj = arc.value();
			else
				return std::nullopt;
			break;
		}
		case ObjectType::Rect:
		{
			auto rect = make_rect(node, type_name, obj_name);
			if (rect)
				obj = rect.value();
			else
				return std::nullopt;
			break;
		}
		default:
		{
			std::cerr << "unexpected type " <<
				type_name << "; ignoring object.\n";
			return std::nullopt;
		}
	}

	return obj;
}


std::optional<Object> Scene::make_circle(YAML::Node& node, std::string type_name,
	std::optional<std::string> obj_name) const
{
	Object obj;

	auto center = get_point(node["center"]);
	obj = Object(type_name, {center});

	auto radius = get_int(node["radius"]);
	if (radius) {
		obj.radius = radius.value();
	} else {
		std::cerr << invalid_value_err("radius",
			obj_name, type_name);
		return std::nullopt;
	}

	return obj;
}

std::optional<Object> Scene::make_arc(YAML::Node& node, std::string type_name,
	std::optional<std::string> obj_name) const
{
	auto obj = make_circle(node, type_name, obj_name);
	if (obj)
		obj.value().arc = get_arc(node["arc"]);
	return obj;
}

std::optional<Object> Scene::make_rect(YAML::Node& node, std::string type_name,
	std::optional<std::string> obj_name) const
{
	auto c = get_point(node["corner"]);
	int width, height;

	auto o_width = get_int(node["width"]);
	if (not o_width) {
		std::cerr << invalid_value_err("width",
			obj_name, type_name);
		return std::nullopt;
	} else {
		width = o_width.value();
	}

	auto o_height = get_int(node["height"]);
	if (not o_height) {
		std::cerr << invalid_value_err("height",
			obj_name, type_name);
		return std::nullopt;
	} else {
		height = o_height.value();
	}

	std::vector<Point> points = {
		{c.x, c.y},
		{c.x+width, c.y},
		{c.x+width, c.y-height},
		{c.x, c.y-height},
		{c.x, c.y}
	};

	return Object(type_name, points);
}

Point Scene::get_point(YAML::Node node) const
{
	Point p = {0, 0};
	if (node.IsSequence()) {
		for (auto i = 0; i < 2; ++i) {
			auto o_int = get_int(node[i]);
			if (o_int) {
				p[i] = o_int.value();
			} else {
				if (node[i]) {
					std::cerr << "Invalid or unkown value \""
						<< node[i].as<std::string>() << "\"";
				} else {
					std::cerr << "Missing value";
				}
				std::cerr << " in position " 
					<< ((i==0) ? "\"x\"" : "\"y\"")
					<< " of point; replacing with "
					<< p[i] << ".\n";
			}
		}
	} else if (node.IsScalar()) {
		auto name = node.as<std::string>();
		try {
			p = points.at(name);
		} catch (const std::out_of_range& e) {
			std::cerr << "Unkown point " << name
				<< "; replacing with " << p
				<< "\n";
		}
	} else {
		std::cerr << "Invalid point;"
		<< " replacing with "<< p << "\n";
	}

	return p;
}

Arc Scene::get_arc(YAML::Node node) const
{
	Arc arc = {0, 1};
	if (node.IsSequence()) {
		for (auto i = 0; i < 2; ++i) {
			auto o_float = get_float(node[i]);
			if (o_float) {
				arc[i] = o_float.value();
			} else {
				if (node[i]) {
					std::cerr << "Invalid or unkown value \""
						<< node[i].as<std::string>() << "\"";
				} else {
					std::cerr << "Missing value";
				}
				std::cerr << " in position " 
					<< ((i==0) ? "\"left\"" : "\"right\"")
					<< " of arc; replacing with "
					<< arc[i] << ".\n";
			}
		}
	} else if (node.IsScalar()) {
		auto name = node.as<std::string>();
		try {
			arc = arcs.at(name);
		} catch (const std::out_of_range& e) {
			std::cerr << "Unkown arc " << name
				<< "; replacing with " << arc
				<< "\n";
		}
	} else {
		std::cerr << "Invalid arc;"
		<< " replacing with "<< arc << "\n";
	}

	if (arc.x < 0 or arc.x > 1 or arc.y < 0 or arc.y > 1) {
		std::cerr << "Invalid arc " << arc.x << ", " << arc.y;
		arc = Arc(0, 1);
		std::cerr << "; using " << arc.x << ", " << arc.y
			<< " instead.\n";
	}

	return arc;
}

Color Scene::get_color(YAML::Node node) const
{
	Color c = DEFAULT_LINE;
	if (node.IsSequence()) {
		std::array<double, 3> value = {0};
		for (auto i = 0; i < 3; ++i) {
			auto o_float = get_float(node[i]);
			if (o_float) {
				value[i] = o_float.value();
			} else {
				if (node[i]) {
					std::cerr << "Invalid or unkown value \""
						<< node[i].as<std::string>() << "\"";
				} else {
					std::cerr << "Missing value";
				}
				std::cerr << " in position " << i
					<< " of color; replacing with "
					<< c[i] << ".\n";
			}
		}

		return {value[0], value[1], value[2]};
	} else if (node.IsScalar()) {
		auto name = node.as<std::string>();
		try {
			c = palette.from_string(name);
		} catch (const std::out_of_range& e) {
			std::cerr << "Unkown color " << name
				<< "; replacing with " << c
				<< "\n";
		}
	} else {
		std::cerr << "Invalid color;"
			<< " replacing with "<< c << "\n";
	}

	return c;
}

std::optional<int> Scene::get_int(YAML::Node node) const
{
	if (not node)
		return std::nullopt;
	
	try {
		return node.as<int>();
	} catch (const YAML::BadConversion& e) {
		try {
			auto name = node.as<std::string>();
			return int_vars.at(name);
		} catch (const std::out_of_range& e) {
			return std::nullopt;
		}
	}
}

std::optional<double> Scene::get_float(YAML::Node node) const
{
	if (not node)
		return std::nullopt;

	try {
		return node.as<double>();
	} catch (const YAML::BadConversion& e) {
		try {
			auto name = node.as<std::string>();
			return float_vars.at(name);
		} catch (const std::out_of_range& e) {
			return std::nullopt;
		}
	}
}

void Scene::load_fill(YAML::Node& node, Object& obj) const
{
	if (node["fill"] or node["fill_color"] or node["flood_points"]) {
		if (node["fill_color"]) {
			obj.fill_color = get_color(node["fill_color"]);
		} else {
			obj.fill_color = Scene::DEFAULT_FILL;
		}

		if (node["fill"]) {
			auto fill_n = node["fill"].as<std::string>();
			try {
				obj.fill = Object::fill_names.at(fill_n);
			} catch (const std::out_of_range& e) {
				obj.fill = std::nullopt;
				std::cerr << "Invalid fill method "
					<< fill_n << "; ";
			}
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

		if (not obj.fill) {
			std::cerr << "not filling ";
			if (obj.name) {
				std::cerr << "object \"" << obj.name.value()
					<< "\".\n";
			} else {
				std::cerr << obj.type_name << " object\n";
			}
		}
	}
}

void Scene::load_collections(YAML::Node config)
{
	for (auto color : config["colors"]) {
		std::string name;
		std::array<double, 3> value;

		auto elem = get_element<decltype(value)>(color, "color");
		if (elem) {
			std::tie(name, value) = elem.value();
			palette.add_color(name, {value[0], value[1], value[2]});
		}
	}

	for (auto point : config["points"]) {
		std::string name;
		ipair_t value;

		auto elem = get_element<decltype(value)>(point, "point");
		if (elem) {
			std::tie(name, value) = elem.value();
			points[name] = Point(value);
		}
	}

	for (auto arc : config["arcs"]) {
		std::string name;
		dpair_t value;

		auto elem = get_element<decltype(value)>(arc, "arc");
		if (elem) {
			std::tie(name, value) = elem.value();
			arcs[name] = Arc(value);
		}
	}

	for (auto var : config["variables"]) {
		std::string type_name;

		if (var.IsSequence()) {
			type_name = var[0].as<std::string>();
		} else if (var.IsMap()) {
			if (var["type"]) {
				type_name = var["type"].as<std::string>();
			} else {
				std::cerr << "variable definition is missing type;"
					<< " ignoring definition.\n";
				continue;
			}
		} else {
			continue;
		}

		if (type_name == "integer") {
			auto elem = get_element<int>(var, "variable", true);
			if (elem) {
				auto [name, value] = elem.value();
				int_vars[name] = value;
			}
		} else if (type_name == "float") {
			auto elem = get_element<double>(var, "variable", true);
			if (elem) {
				auto [name, value] = elem.value();
				float_vars[name] = value;
			}
		} else {
			std::cerr << "variable definition has an"
				<< " invalid type \"" << type_name 
				<< "\"; ignoring definition.\n";
		}
	}
}


const std::string Scene::invalid_value_err(std::string field,
	std::optional<std::string> obj_name, std::string type_name)
{
	std::string err;
	err += "Invalid or missing " + field + " value; ";
	if (obj_name) {
		err += "ignoring object \"" + obj_name.value() + "\".\n";
	} else {
		err += "ignoring " + type_name + " object.\n";
	}
	return err;
}