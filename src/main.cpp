#include <iostream>
#include "Rasterizer.h"

int main()
{
	Rasterizer cobra;
	cobra.load_scene("../config.yaml");
	std::cout << "loaded scene with " 
		<< cobra.scene.objects.size();
	std::cout << " objects\n";

	cobra.rasterize();
	std::cout << "rasterized\n";

	cobra.write_image("../imgs/i1.png");
	std::cout << "done\n";
}