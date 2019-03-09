#include <iostream>
#include "Colors.h"

std::map<std::string, Color> Colors::values = {
    {"white", Colors::WHITE},
    {"black", Colors::BLACK},
    {"mid_gray", Colors::MID_GRAY},
    {"red", Colors::RED},
    {"green", Colors::GREEN},
    {"blue", Colors::BLUE},
};

const Color Colors::from_string(std::string name) {
	return values.at(name);
}

void Colors::add_color(std::string name, Color value) {
	values[name] = value;
}