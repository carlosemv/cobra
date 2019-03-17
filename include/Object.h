#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <optional>
#include <string>
#include <vector>
#include <list>
#include <initializer_list>

#include "Point.h"
#include "Edge.h"
#include "Arc.h"
#include "Colors.h"

enum class ObjectType {
	Line,
	Polyline,
	Polygon,
	Rect,
	Circle,
	Arc,
	Invalid
};

enum class FillMethod {
	Scanline,
	Flood
};

class Object
{
public:
	Object() : type(ObjectType::Invalid) {}
	Object(std::string _tn, std::initializer_list<Point> _p);
	Object(std::string _tn, std::vector<Point> _v);
	bool is_closed() const;
	bool is_polygon() const;

	std::string type_name;
	ObjectType type;
	std::vector<Point> points;
	Color line_color;

	std::optional<unsigned> radius;
	std::optional<Arc> arc;

	std::optional<std::list<Edge>> edges;
	std::optional<FillMethod> fill;
	std::optional<Color> fill_color;
	std::optional<std::vector<Point>> flood_points;
	std::optional<std::string> name;

	static const std::map<std::string, ObjectType> type_names;
	static const std::map<std::string, FillMethod> fill_names;
};

#endif