#include "ppm.hpp"

#include <fstream>

int clamp_pixel_value(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

void write_ppm(vec4** image_buffer, int width, int height, std::string filename)
{
	std::ofstream file;

	file.open(filename.c_str());

	file << "P3" << std::endl;
	file << "# " << filename << std::endl;
	file << width << " " << height << std::endl;
	file << "255" << std::endl;

	for (int j = height - 1; j >= 0; j--)
	{
		for (int i = 0; i < width; i++)
		{
			vec4 value = image_buffer[i][j];
			file << clamp_pixel_value(value[0]) << " "
				<< clamp_pixel_value(value[1]) << " "
				<< clamp_pixel_value(value[2]) << " ";
		}
		file << std::endl;
	}
	file.close();
}