#include <string>
#include <vector>
#include <utility>
#include "Scene.h"
#include "mat4.hpp"
#include "ppm.hpp"
#include "line.hpp"

mat4 get_projection_matrix(Camera& c)
{
	mat4 vp = mat4::identity();
	double l = c.left, r = c.right, t = c.top, b = c.bottom;
	double n = c.near, f = c.far;
	vec4 u = c.u, v = c.v, w = c.w, e = c.pos;
	u[3] = 0, v[3] = 0, w[3] = 0, e[3] = 0;

	mat4c cam = mat4c(
		row4{ u[0], u[1], u[2], -dot4(u, e)},
		row4{ v[0], v[1], v[2], -dot4(v, e)},
		row4{ w[0], w[1], w[2], -dot4(w, e)},
		row4{ 0, 0, 0, 1 }
	);

	mat4c orth = mat4c(
		row4{ 2.0 / (r - l), 0, 0, -((r + l) / (r - l)) },
		row4{ 0, 2.0 / (t - b), 0, -((t + b) / (t - b)) },
		row4{ 0, 0, -2.0 / (f - n), -((f + n) / (f - n)) },
		row4{ 0, 0, 0, 1 }
	);

	if (c.projection_type == ORTHOGRAPHIC)
	{
		vp = orth * cam * vp;
	}
	else
	{
		mat4c per = mat4(
			row4{ (2 * n) / (r - l), 0, (r + l) / (r - l), 0 },
			row4{ 0, (2 * n) / (t - b), (t + b) / (t - b), 0 },
			row4{ 0, 0, -(f + n) / (f - n), -(2 * f * n) / (f - n) },
			row4{ 0, 0, -1, 0 }
		);
		vp = per * cam * vp;
	}

	return vp;
}

mat4 get_viewport_matrix(Camera& c)
{
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
			if (camera.projection_type == PERSPECTIVE)
				for (auto& coord : tri.v)
					coord /= coord[3];

			//viewport transformation
			for (auto& coord : tri.v)
			{
				coord = viewport_matrix * coord;
				coord[0] += 0.5;
				coord[1] += 0.5;
			}

			//draw
			auto v0 = tri.v[0], v1 = tri.v[1], v2 = tri.v[2];
			auto v0_c = tri.color[0], v1_c = tri.color[1], v2_c = tri.color[2];
			clip_line(v0[0], v0[1], v1[0], v1[1], v0_c, v1_c, image_buffer, camera.width, camera.height);
			clip_line(v1[0], v1[1], v2[0], v2[1], v1_c, v2_c, image_buffer, camera.width, camera.height);
			clip_line(v2[0], v2[1], v0[0], v0[1], v2_c, v0_c, image_buffer, camera.width, camera.height);
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
			{
				image_buffer[i] = new vec4[camera.height];
				for (int j = 0; j < camera.height; j++)
					image_buffer[i][j] = scene.background_color;
			}

			render_camera(scene, camera, image_buffer);

			write_ppm(image_buffer, camera.width, camera.height, camera.output_file_name);
			ppm_to_png(camera.output_file_name);
        }

        return EXIT_SUCCESS;
    }
}