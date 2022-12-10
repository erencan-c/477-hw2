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

	std::vector< std::vector<vec4> > image;
	std::vector< Camera > cameras;
	std::vector< vec4 > colorsOfVertices;
	std::vector< Mesh > meshes;

	Scene(const char *xmlPath);

	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(std::string ppmFileName, int osType);
};

#endif
