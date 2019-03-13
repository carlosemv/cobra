#include "Object.h"

const std::map<std::string, ObjectType> Object::type_names = {
	{"line", ObjectType::Line},
	{"polyline", ObjectType::Polyline},
	{"polygon", ObjectType::Polygon},
	{"circle", ObjectType::Circle},
	{"rect", ObjectType::Rect},
};

const std::map<std::string, FillMethod> Object::fill_names = {
	{"scanline", FillMethod::Scanline},
	{"flood", FillMethod::Flood},
};

Object::Object(std::string _tn, std::initializer_list<Point> _p) :
	type_name(_tn), type(type_names.at(_tn)), points(_p)
{}

Object::Object(std::string _tn, std::vector<Point> _v) :
	type_name(_tn), type(type_names.at(_tn)), points(_v)
{}

bool Object::is_closed()
{
	switch (type) {
		case ObjectType::Polygon:
		case ObjectType::Circle:
		case ObjectType::Rect:
			return true;
		case ObjectType::Line:
		case ObjectType::Polyline:
		case ObjectType::Invalid:
		default:
			return false;
	}
}