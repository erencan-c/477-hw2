#ifndef _SCENE_H_
#define _SCENE_H_

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Camera.h"
#include "Mesh.h"
#include "Triangle.h"

using namespace std;

class Scene
{
public:
	vec4 background_color;
	bool culling_enabled;

	vector< vector<vec4> > image;
	vector< Camera > cameras;
	vector< vec4 > colorsOfVertices;
	vector< Mesh > meshes;

	Scene(const char *xmlPath);

	int makeBetweenZeroAnd255(double value);
	void writeImageToPPMFile(Camera* camera);
	void convertPPMToPNG(string ppmFileName, int osType);
};

#endif
