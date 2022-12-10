#include <fstream>
#include <string>
#include <vector>
#include "Scene.h"
#include "mat4.hpp"

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

mat4 get_projection_matrix(Camera& c)
{
	mat4 vp;
	double l = c.left, r = c.right, t = c.top, b = c.bottom;
	double n = c.near, f = c.far;

	mat4c orth = mat4(
		row4{ 2.0 / (r - l), 0, 0, -((r + l) / (r - l)) },
		row4{ 0, 2.0 / (t - b), 0, -((t + b) / (t - b)) },
		row4{ 0, 0, -2.0 / (f - n), -((f + n) / (f - n)) },
		row4{ 0, 0, 0, 1 }
	);
	vp = orth * vp;

	if (c.projection_type == PERSPECTIVE)
	{
		mat4c pers = mat4(
			row4{ (2 * n) / (r - l), 0, (r + l) / (r - l), 0 },
			row4{ 0, (2 * n) / (t - b), (t + b) / (t - b), 0 },
			row4{ 0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n) },
			row4{ 0, 0, -1, 0 }
		);
		vp = vp * pers;
	}

	return vp;
}

mat4 get_viewport_matrix(Camera& c)
{
	double l = c.left, r = c.right, t = c.top, b = c.bottom;
	double n = c.near, f = c.far;
	double nx = c.width, ny = c.height;
	return mat4(
		row4{ nx / 2.0, 0, 0, (nx - 1.0) / 2.0 },
		row4{ 0, ny / 2.0, 0, (ny - 1.0) / 2.0 },
		row4{ 0, 0, 0.5, 0.5 },
		row4{ 0, 0, 0, 1 }
	);
}

void render_camera(Scene& scene, Camera& camera, vec4** image_buffer)
{
	mat4 proj_matrix = get_projection_matrix(camera);
	mat4 viewport_matrix = get_viewport_matrix(camera);

	for (auto& mesh : scene.meshes)
	{
		for (auto tri : mesh.triangles)
		{
			//projection
			for (auto& coord : tri.v) 
				coord = proj_matrix * coord;

			//perspective divide
			for (auto& coord : tri.v)
				coord /= coord[3];

			//viewport transformation
			for (auto& coord : tri.v)
				coord = viewport_matrix * coord;


		}
	}
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Please run the rasterizer as:" << std::endl
             << "\t./rasterizer <input_file_name>" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        Scene scene(argv[1]);

        for (auto& camera : scene.cameras)
        {
			vec4** image_buffer = new vec4*[camera.width];
			for (int i = 0; i < camera.width; i++)
				image_buffer[i] = new vec4[camera.height];

			render_camera(scene, camera, image_buffer);

			write_ppm(image_buffer, camera.width, camera.height, camera.output_file_name);
        }

        return EXIT_SUCCESS;
    }
}