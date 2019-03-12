#ifndef __RASTERIZER_H__
#define __RASTERIZER_H__

#include <iostream>
#include "Scene.h"
#include "Canvas.h"

class Rasterizer
{
public:
	void load_scene(std::string _f) {this->scene = Scene(_f);}
	void rasterize();
	void write_image(std::string _out) {this->canvas.write_png(_out);}

	Scene scene;
	Canvas canvas;
};

#endif