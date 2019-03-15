#ifndef __SCENE_H__
#define __SCENE_H__

#include <iostream>
#include "yaml-cpp/yaml.h"
#include "Object.h"
#include "Edge.h"

class Scene
{
public:
	Scene() : height(0), width(0), bg_color(DEFAULT_BG) {}
	Scene(std::string config_file);

	unsigned height, width;
	Color bg_color;
	std::vector<Object> objects;

	static constexpr auto DEFAULT_BG = Colors::WHITE;
	static constexpr auto DEFAULT_LINE = Colors::BLACK;
	static constexpr auto DEFAULT_FILL = DEFAULT_LINE;

private:
	Colors palette;
	std::map<std::string, Point> points;

	Point get_point(YAML::Node node) const;
};

#endif