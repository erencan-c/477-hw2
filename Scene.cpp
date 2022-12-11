#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <cmath>
#include <map>

#include "Scene.h"
#include "mat4.hpp"
#include "tinyxml2.h"

using namespace tinyxml2;

class ParseError {};

Scene::Scene(const char *xmlPath)
{
	std::vector< std::pair<vec4,vec4> > v;
	std::map<int, mat4> translations;
	std::map<int, mat4> rotations;
	std::map<int, mat4> scalings;

	const char *str;
	XMLDocument xmlDoc;
	XMLElement *pElement;

	if (xmlDoc.LoadFile(xmlPath) != XML_SUCCESS)
	{
		throw ParseError();
	}

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
		cam.gaze[3] = 0;
		cam.v[3] = 0;

		{
			int zort;
			pCamera->QueryIntAttribute("id", &zort); 
		}

		// read projection type
		str = pCamera->Attribute("type");

		if (strcmp(str, "orthographic") == 0) {
			cam.projection_type = ORTHOGRAPHIC;
		}
		else {
			cam.projection_type = PERSPECTIVE;
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
		cam.u = CROSS_PRODUCT(cam.gaze, cam.v);
		cam.u = normalize4(cam.u);

		cam.w = -cam.gaze;
		cam.v = CROSS_PRODUCT(cam.u, cam.gaze);
		cam.v = normalize4(cam.v);

		camElement = pCamera->FirstChildElement("ImagePlane");
		str = camElement->GetText();
		sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
			   &cam.left, &cam.right, &cam.bottom, &cam.top,
			   &cam.near, &cam.far, &cam.width, &cam.height);

		camElement = pCamera->FirstChildElement("OutputName");
		str = camElement->GetText();
		cam.output_file_name = str;

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
		vertex[3] = 1;
		color[3] = 255;

		str = pVertex->Attribute("position");
		sscanf(str, "%lf %lf %lf", &vertex[0], &vertex[1], &vertex[2]);

		str = pVertex->Attribute("color");
		sscanf(str, "%lf %lf %lf", &color[0], &color[1], &color[2]);

		v.push_back({vertex, color});

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

		{
			int zort;
			pMesh->QueryIntAttribute("id", &zort);
		}

		// read projection type
		str = pMesh->Attribute("type");

		if (strcmp(str, "wireframe") == 0) {
			mesh.type = WIREFRAME;
		}
		else {
			mesh.type = SOLID;
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
				v1--, v2--, v3--;
				auto tri = Triangle{
					{composite_transformation * v[v1].first, composite_transformation * v[v2].first, composite_transformation * v[v3].first},
					{v[v1].second, v[v2].second, v[v3].second} };
				mesh.triangles.push_back(tri);
			}
			row = strtok(NULL, "\n");
		}
		meshes.push_back(mesh);

		pMesh = pMesh->NextSiblingElement("Mesh");
	}
}