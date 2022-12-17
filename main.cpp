#include <string>
#include <vector>
#include <utility>
#include "Scene.h"
#include "mat4.hpp"
#include "ppm.hpp"
#include "line.hpp"
#include <cfloat>

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

vec4 get_triangle_normal(const Triangle& tri)
{
	auto edge1 = tri.v[2] - tri.v[0];
	auto edge2 = tri.v[1] - tri.v[0];
	return cross4(edge2, edge1);
}

vec4 get_triangle_center(const Triangle& tri)
{
	return (tri.v[0] + tri.v[1] + tri.v[2]) / 3.0;
}

std::pair<vec4, vec4> get_triangle_bounds(const Triangle& tri)
{
	vec4 curmin = vec4{ DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX }, curmax = {DBL_MIN, DBL_MIN, DBL_MIN, DBL_MIN };

	curmin = min4(curmin, tri.v[0]);
	curmin = min4(curmin, tri.v[1]);
	curmin = min4(curmin, tri.v[2]);

	curmax = max4(curmax, tri.v[0]);
	curmax = max4(curmax, tri.v[1]);
	curmax = max4(curmax, tri.v[2]);

	return { curmin, curmax };
}

double sample_line_equation(vec4c v0, vec4c v1, double x, double y)
{
	double x0 = v0[0], y0 = v0[1], x1 = v1[0], y1 = v1[1];
	return x * (y0 - y1) + y * (x1 - x0) + (x0 * y1) - (y0 * x1);
}

void draw_solid(const Triangle& tri, vec4** image_buffer, int width, int height)
{
	auto v0 = tri.v[0], v1 = tri.v[1], v2 = tri.v[2];
	auto v0_c = tri.color[0], v1_c = tri.color[1], v2_c = tri.color[2];
	auto [minb, maxb] = get_triangle_bounds(tri);
	for (int x = minb[0]; x < maxb[0]; x++)
	{
		for (int y = minb[1]; y < maxb[1]; y++)
		{
			double alpha = sample_line_equation(v1, v2, x, y) / sample_line_equation(v1, v2, v0[0], v0[1]);
			double beta = sample_line_equation(v2, v0, x, y) / sample_line_equation(v2, v0, v1[0], v1[1]);
			double gamma = sample_line_equation(v0, v1, x, y) / sample_line_equation(v0, v1, v2[0], v2[1]);

			if (alpha >= 0 && beta >= 0 && gamma >= 0 && x >= 0 && y >= 0 && x < width && y < height)
			{
				vec4 intp_color = alpha * v0_c + beta * v1_c + gamma * v2_c;
				image_buffer[x][y] = intp_color;
			}
		}
	}
}

void render_camera(Scene& scene, Camera& camera, vec4** image_buffer)
{
	mat4 proj_matrix = get_projection_matrix(camera);
	mat4 viewport_matrix = get_viewport_matrix(camera);

	/*
	clip_line(350, 350, 350, 699, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 525, 699, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 699, 699, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 699, 525, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 699, 350, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 699, 175, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 699, 0, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 525, 0, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 350, 0, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 175, 0, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 0, 0, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 0, 175, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 0, 350, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 0, 525, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 0, 699, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	clip_line(350, 350, 175, 699, vec4{ 0,0,0,1 }, vec4{ 0,0,0,1 }, image_buffer, camera.width, camera.height);
	*/
	
	for (auto& mesh : scene.meshes)
	{
		for (auto tri : mesh.triangles)
		{
			if (scene.culling_enabled)
			{
				auto cull = dot4(get_triangle_normal(tri), camera.pos - get_triangle_center(tri)) <= 0.0;
				if (camera.projection_type == ORTHOGRAPHIC)
					cull = !cull;
				if (cull)
					continue;
			}

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

			if (mesh.type == WIREFRAME)
			{
				//draw wireframe
				auto v0 = tri.v[0], v1 = tri.v[1], v2 = tri.v[2];
				auto v0_c = tri.color[0], v1_c = tri.color[1], v2_c = tri.color[2];
				clip_line(v0[0], v0[1], v1[0], v1[1], v0_c, v1_c, image_buffer, camera.width, camera.height);
				clip_line(v1[0], v1[1], v2[0], v2[1], v1_c, v2_c, image_buffer, camera.width, camera.height);
				clip_line(v2[0], v2[1], v0[0], v0[1], v2_c, v0_c, image_buffer, camera.width, camera.height);
			}
			else
			{
				//draw solid
				draw_solid(tri, image_buffer, camera.width, camera.height);
			}
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
			//ppm_to_png(camera.output_file_name);
        }

        return EXIT_SUCCESS;
    }
}