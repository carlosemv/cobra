#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include <iostream>
#include "Scene.h"
#include "Canvas.h"

class Rasterizer
{
public:
	void load_scene(std::string _f) {load_conf(_f); this->scene = Scene(_f);}
	void rasterize();
	void write_image(std::string _out) {this->canvas.write_png(_out);}

private:
	Scene scene;
	Canvas canvas;
	bool antialiasing;

	static const std::vector<int> GAUSSIAN;

	void load_conf(std::string _f);
};

#endif