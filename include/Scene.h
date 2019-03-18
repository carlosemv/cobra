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
	std::map<std::string, Arc> arcs;
	std::map<std::string, int> int_vars;
	std::map<std::string, double> float_vars;

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

	std::optional<int> get_int(YAML::Node node) const;
	std::optional<double> get_float(YAML::Node node) const;

	static const std::string invalid_value_err(std::string field,
		std::optional<std::string> obj_name, std::string type_name);

	template <class T, class F>
	std::optional<T> get_value (YAML::Node& node, F field) const
	{
		T value;
		if (node[field]) {
			try {
				value = node[field].template as<T>();
			} catch (const YAML::BadConversion& e) {
				return std::nullopt;
			}
		} else {
			return std::nullopt;
		}

		return value;
	}

	template <class T>
	std::optional<std::pair<std::string, T>> get_element(
		YAML::Node node, std::string elem_name, bool has_type=false)
	{
		std::string name;
		T value;

		bool invalid_name = false;
		bool invalid_value = false;

		if (node.IsMap() or node.IsSequence()) {
			std::optional<std::string> o_name;
			std::optional<T> o_value;
			if (node.IsMap()) {
				o_name = get_value<std::string>(node, "name");
				o_value = get_value<T>(node, "value");
			} else if (node.IsSequence()) {
				o_name = get_value<std::string>(node, 0+has_type);
				o_value = get_value<T>(node, 1+has_type);
			}

			if (not o_name)
				invalid_name = true;
			else
				name = o_name.value();

			if (not o_value)
				invalid_value = true;
			else
				value = o_value.value();
		} else if (node.IsScalar()) {
			std::cerr << "Ignoring invalid " 
				<< elem_name << " definition \""
				<< node.template as<std::string>() << "\".\n";
			return std::nullopt;
		} else {
			return std::nullopt;
		}

		if (invalid_name) {
			std::cerr << elem_name 
				<< " definition has an invalid or missing name;"
				<< " ignoring definition.\n";
		} else if (invalid_value) {
			std::cerr << elem_name << " \"" << name << "\""
				<< " has an invalid or missing value;"
				<< " ignoring definition.\n";
		} else {
			return make_pair(name, value);
		}

		return std::nullopt;
	}
};

#endif