#include "Scene.h"

Scene::Scene(std::string config_file)
{
	YAML::Node config = YAML::LoadFile(config_file);
	this->height = config["height"].as<int>();
	this->width = config["width"].as<int>();

	if (config["background"]) {
		auto bg = config["background"].as<std::string>();
		this->bg_color = Colors::from_string(bg);
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
			default: 
				std::cout << "unexpected type!\n"; break;
		}

		std::cout << obj.type_name << std::endl;

		if (node["color"]) {
			auto lcolor = node["color"].as<std::string>();
			obj.line_color = Colors::from_string(lcolor);
		} else {
			obj.line_color = Scene::DEFAULT_LINE;
		}

		if (node["fill"]) {
			auto fcolor = node["fill"].as<std::string>();
			obj.fill_color = Colors::from_string(fcolor);
		}

		if (node["name"]) {
			obj.name = node["name"].as<std::string>();
		}

		this->objects.push_back(obj);
	}
}