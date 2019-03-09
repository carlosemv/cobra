#include <iostream>
#include "yaml-cpp/yaml.h"
#include "Canvas.h"

int main()
{
	YAML::Node config = YAML::LoadFile("../config.yaml");
	auto bg = config["background"].as<std::string>();
	auto height = config["height"].as<int>();
	auto width = config["width"].as<int>();

	Canvas canvas(height, width);
	canvas.fill(Colors::from_string(bg));

	// std::cout << config["objects"].Type() << std::endl;
	for (auto node : config["objects"]) {
		std::cout << node.Type() << std::endl;
		std::cout << node["type"].as<std::string>() << std::endl;

		auto start = node["start"].as<ipair_t>();
		auto end = node["end"].as<ipair_t>();
		auto lcolor = node["color"].as<std::string>();
		canvas.paint_line(start, end, 
			Colors::from_string(lcolor));
	}

	canvas.write_png("../imgs/i1.png");
}