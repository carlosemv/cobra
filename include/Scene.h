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
	std::map<std::string, Point> arcs;

	void load_collections(YAML::Node config);
	void load_fill(YAML::Node& node, Object& obj) const;

	std::optional<Object> make_object(YAML::Node& node, std::string type_name,
		std::optional<std::string> obj_name) const;
	std::optional<Object> make_circle(YAML::Node& node, std::string type_name,
		std::optional<std::string> obj_name) const;
	std::optional<Object> make_arc(YAML::Node& node, std::string type_name,
		std::optional<std::string> obj_name) const;
	std::optional<Object> make_rect(YAML::Node& node, std::string type_name,
		std::optional<std::string> obj_name) const;

	Point get_point(YAML::Node node) const;
	Color get_color(YAML::Node node) const;
	Arc get_arc(YAML::Node node) const;

	static const std::string invalid_value_err(std::string field,
		std::optional<std::string> obj_name, std::string type_name);

	template <class T>
	std::optional<T> get_value (YAML::Node& node, std::string field) const
	{
		T value;
		if (node[field]) {
			try {
				value = node[field].as<T>();
			} catch (const YAML::BadConversion& e) {
				return std::nullopt;
			}
		} else {
			return std::nullopt;
		}

		return value;
	}
};

#endif