#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cmath>
#include <map>

#include "Scene.h"
#include "mat4.hpp"
#include "Triangle.h"
#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

class ParseError {};

Scene::Scene(const char *xmlPath)
{
	vector< std::pair<vec4,vec4> > vertices;
	map<int, mat4> translations;
	map<int, mat4> rotations;
	map<int, mat4> scalings;

	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	xmlDoc.LoadFile(xmlPath);

	XMLNode *pRoot = xmlDoc.FirstChild();

	// read background color
	pElement = pRoot->FirstChildElement("BackgroundColor");
	str = pElement->GetText();
	sscanf(str, "%lf %lf %lf", &background_color[0], &background_color[1], &background_color[2]);

	// read culling
	pElement = pRoot->FirstChildElement("Culling");
	if (pElement != NULL) {
		str = pElement->GetText();
		
		if (strcmp(str, "enabled") == 0) {
			culling_enabled = true;
		}
		else {
			culling_enabled = false;
		}
	}

	// read cameras
	pElement = pRoot->FirstChildElement("Cameras");
	XMLElement *pCamera = pElement->FirstChildElement("Camera");
	XMLElement *camElement;
	while (pCamera != NULL)
	{
		Camera cam;

		pCamera->QueryIntAttribute("id", &cam.cameraId);

		// read projection type
		str = pCamera->Attribute("type");

		if (strcmp(str, "orthographic") == 0) {
			cam.projectionType = 0;
	
		}
		else {
			cam.projectionType = 1;
		}

		camElement = pCamera->FirstChildElement("Position");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam.pos[0], &cam.pos[1], &cam.pos[2]);

		camElement = pCamera->FirstChildElement("Gaze");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam.gaze[0], &cam.gaze[1], &cam.gaze[2]);

		camElement = pCamera->FirstChildElement("Up");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf", &cam.v[0], &cam.v[1], &cam.v[2]);

		cam.gaze = normalize4(cam.gaze);
		cam.u = cross4(cam.gaze, cam.v);
		cam.u = normalize4(cam.u);

		cam.w = -cam.gaze;
		cam.v = cross4(cam.u, cam.gaze);
		cam.v = normalize4(cam.v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam.left, &cam.right, &cam.bottom, &cam.top,
			   &cam.near, &cam.far, &cam.horRes, &cam.verRes);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam.outputFileName = string(str);

		cameras.push_back(cam);

		pCamera = pCamera->NextSiblingElement("Camera");
	}

	// read vertices
	pElement = pRoot->FirstChildElement("Vertices");
	XMLElement *pVertex = pElement->FirstChildElement("Vertex");
	int vertexId = 1;

	while (pVertex != NULL)
	{
		vec4 vertex;
		vec4 color;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", vertex[0], &vertex[1], &vertex[2]);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color[0], &color[1], &color[2]);

		vertices.push_back({vertex, color});
		colorsOfVertices.push_back(color);

		pVertex = pVertex->NextSiblingElement("Vertex");

		vertexId++;
	}

	// read translations
	pElement = pRoot->FirstChildElement("Translations");
	XMLElement *pTranslation = pElement->FirstChildElement("Translation");
	while (pTranslation != NULL)
	{
		int id;
		arr4 xyz;

		pTranslation->QueryIntAttribute("id", &id);

		str = pTranslation->Attribute("value");
		sscanf(str, "%lf %lf %lf", &xyz[0], &xyz[1], &xyz[2]);

		translations[id] = mat4::transition(xyz);

		pTranslation = pTranslation->NextSiblingElement("Translation");
	}

	// read scalings
	pElement = pRoot->FirstChildElement("Scalings");
	XMLElement *pScaling = pElement->FirstChildElement("Scaling");
	while (pScaling != NULL)
	{
		int id;
		arr4 xyz;

		pScaling->QueryIntAttribute("id", &id);
		str = pScaling->Attribute("value");
		sscanf(str, "%lf %lf %lf", &xyz[0], &xyz[1], &xyz[2]);

		scalings[id] = mat4::scaling(xyz);

		pScaling = pScaling->NextSiblingElement("Scaling");
	}

	// read rotations
	pElement = pRoot->FirstChildElement("Rotations");
	XMLElement *pRotation = pElement->FirstChildElement("Rotation");
	while (pRotation != NULL)
	{
		int id;
		double angle;
		vec4 u;

		pRotation->QueryIntAttribute("id", &id);
		str = pRotation->Attribute("value");
		sscanf(str, "%lf %lf %lf %lf", &angle, &u[0], &u[1], &u[2]);

		rotations[id] = mat4::rotation(u, angle);

		pRotation = pRotation->NextSiblingElement("Rotation");
	}

	// read meshes
	pElement = pRoot->FirstChildElement("Meshes");

	XMLElement *pMesh = pElement->FirstChildElement("Mesh");
	XMLElement *meshElement;
	while (pMesh != NULL)
	{
		mat4 composite_transformation;
		Mesh mesh;

		pMesh->QueryIntAttribute("id", &mesh.id);

		// read projection type
		str = pMesh->Attribute("type");

		if (strcmp(str, "wireframe") == 0) {
			mesh.type = 0;
		}
		else {
			mesh.type = 1;
		}

		// read mesh transformations
		XMLElement *pTransformations = pMesh->FirstChildElement("Transformations");
		XMLElement *pTransformation = pTransformations->FirstChildElement("Transformation");

		while (pTransformation != NULL)
		{
			char type;
			int id;

			str = pTransformation->GetText();
			sscanf(str, "%c %d", &type, &id);

			switch(type) {
				case 'r':
					composite_transformation = rotations[id] * composite_transformation;
				break;
				case 't':
					composite_transformation = translations[id] * composite_transformation;
				break;
				case 's':
					composite_transformation = scalings[id] * composite_transformation;
				break;
				default:
					throw ParseError();
			}

			pTransformation = pTransformation->NextSiblingElement("Transformation");
		}

		// read mesh faces
		char *row;
		char *clone_str;
		int v1, v2, v3;
		XMLElement *pFaces = pMesh->FirstChildElement("Faces");
        str = pFaces->GetText();
		clone_str = strdup(str);

		row = strtok(clone_str, "\n");
		while (row != NULL)
		{
			int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);
			
			if (result != EOF) {
				mesh.triangles.push_back(Triangle{
					{composite_transformation * vertices[v1].first, composite_transformation * vertices[v2].first, composite_transformation * vertices[v3].first},
					{vertices[v1].second, vertices[v2].second, vertices[v3].second}
				});
			}
			row = strtok(NULL, "\n");
		}
		meshes.push_back(mesh);

		pMesh = pMesh->NextSiblingElement("Mesh");
	}
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
	if (value >= 255.0)
		return 255;
	if (value <= 0.0)
		return 0;
	return (int)(value);
}

/*
	Writes contents of image (vec4**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera *camera)
{
	ofstream fout;

	fout.open(camera->outputFileName.c_str());

	fout << "P3" << endl;
	fout << "# " << camera->outputFileName << endl;
	fout << camera->horRes << " " << camera->verRes << endl;
	fout << "255" << endl;

	for (int j = camera->verRes - 1; j >= 0; j--)
	{
		for (int i = 0; i < camera->horRes; i++)
		{
			fout << makeBetweenZeroAnd255(image[i][j][0]) << " "
				 << makeBetweenZeroAnd255(image[i][j][1]) << " "
				 << makeBetweenZeroAnd255(image[i][j][2]) << " ";
		}
		fout << endl;
	}
	fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void Scene::convertPPMToPNG(string ppmFileName, int osType)
{
	string command;

	// call command on Ubuntu
	if (osType == 1)
	{
		command = "convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// call command on Windows
	else if (osType == 2)
	{
		command = "magick convert " + ppmFileName + " " + ppmFileName + ".png";
		system(command.c_str());
	}

	// default action - don't do conversion
	else
	{
	}
}