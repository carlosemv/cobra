#include <iostream>
#include "Rasterizer.h"

int main(int argc, char* argv[])
{
	if (argc < 3) {
		std::cerr << "Invalid number of arguments, correct format is: ";
		std::cerr << "executable <scene file> <output file>\n";
		return EXIT_FAILURE;
	}

	Rasterizer cobra;
	std::string scene_file(argv[1]);
	std::string output_file(argv[2]);

	std::cout << "loading scene from " << scene_file << "\n";
	cobra.load_scene(scene_file);
	std::cout << "loaded scene\n";

	std::cout << "rasterizing...\n";
	cobra.rasterize();
	std::cout << "rasterized\n";

	std::cout << "writing image " << output_file << "\n";
	cobra.write_image(output_file);
	std::cout << "done\n";

	return EXIT_SUCCESS;
}