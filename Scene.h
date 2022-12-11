#ifndef _SCENE_H_
#define _SCENE_H_

#include <string>
#include <vector>

#include "geometry.hpp"

class Scene
{
public:
	vec4 background_color;
	bool culling_enabled;

	std::vector< Camera > cameras;
	std::vector< Mesh > meshes;

	Scene(const char *xmlPath);
};

#endif
