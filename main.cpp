#include <fstream>
#include <string>
#include <vector>
#include <utility>
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

void draw_line(int x1, int y1, int x2, int y2, const vec4c start_color, const vec4c end_color, vec4** image_buffer, const int width, const int height)
{
	if (x1 > x2)
	{
		draw_line(x2, y2, x1, y1, end_color, start_color, image_buffer, width, height);
		return;
	}

	int m = 0;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = dx - 2 * dy;
	int new_d = dy - 2 * dx;
	int y = y1;
	int x = x1;
	float line_slope = (float)dy / dx;

	if (dy < 0)
	{
		m = -1;
		dy = -dy;
	}
	else if (dy >= 0)
	{
		m = 1;
	}

	if (line_slope <= 1.0 && dx != 0)
	{
		for (x = x1; x < x2; x++)
		{
			//todo use line clipping (liang-barsky) here
			if (x >= 0 && y >= 0 && x < width && y < height)
				image_buffer[x][y] = vec4{ 255, 255, 255, 255 };

			if (d <= 0)
			{
				y += m;
				d += 2 * (dx - dy);
			}
			else
			{
				d += -2 * dy;
			}
		}

	}
	else
	{
		if (y1 > y2)
			std::swap(y1, y2);

		for (y = y1; y < y2; y++)
		{
			if (x >= 0 && y >= 0 && x < width && y < height)
				image_buffer[x][y] = vec4{ 255, 255, 255, 255 };

			if (new_d <= 0)
			{
				x += m;
				new_d += 2 * (dy - dx);
			}
			else
			{
				new_d += -2 * dx;
			}
		}

	}
}


void render_camera(Scene& scene, Camera& camera, vec4** image_buffer)
{
	mat4 proj_matrix = get_projection_matrix(camera);
	mat4 viewport_matrix = get_viewport_matrix(camera);

	draw_line( image_buffer, camera.width, camera.height);
	//draw_line(80, 50, 90, 150, image_buffer, camera.width, camera.height);

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
			{
				coord = viewport_matrix * coord;
				coord[0] += 0.5;
				coord[1] += 0.5;
			}

			auto v0 = tri.v[0], v1 = tri.v[1], v2 = tri.v[2];
			draw_line(v0[0], v0[1], v1[0], v1[1], image_buffer, camera.width, camera.height);
			draw_line(v1[0], v1[1], v2[0], v2[1], image_buffer, camera.width, camera.height);
			draw_line(v2[0], v2[1], v0[0], v0[1], image_buffer, camera.width, camera.height);
		}
	}
}

void ppm_to_png(std::string ppm_file)
{
	std::string command = "convert " + ppm_file + " " + ppm_file + ".png";
	system(command.c_str());
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
			ppm_to_png(camera.output_file_name);
        }

        return EXIT_SUCCESS;
    }
}