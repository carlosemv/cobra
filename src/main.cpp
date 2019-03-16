#include <iostream>
#include "Rasterizer.h"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Invalid number of arguments: ";
		std::cerr << "scene configuration filename"\
		" must follow executable name\n";
		return EXIT_FAILURE;
	}

	Rasterizer cobra;
	std::string scene_file(argv[1]);

	std::cout << "loading scene from " << scene_file << "\n";
	cobra.load_scene(scene_file);
	std::cout << "loaded scene\n";

	std::cout << "rasterizing...\n";
	cobra.rasterize();
	std::cout << "rasterized\n";

	std::cout << "writing image " << cobra.out_file << "\n";
	cobra.write_image();
	std::cout << "done\n";

	return EXIT_SUCCESS;
}