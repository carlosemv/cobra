#include <iostream>
#include "Rasterizer.h"

int main()
{
	Rasterizer cobra;
	cobra.load_scene("../big.yaml");
	std::cout << "loaded scene\n";

	cobra.rasterize();
	std::cout << "rasterized\n";

	cobra.write_image();
	std::cout << "done\n";
}