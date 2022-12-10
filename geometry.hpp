#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <vector>
#include <array>
#include <string>
#include "vec.hpp"

enum RenderType
{
    WIREFRAME, SOLID
};

enum ProjectionType
{
    ORTHOGRAPHIC, PERSPECTIVE
};

struct Triangle
{
    std::array<vec4, 3> v;
    std::array<vec4, 3> color;
};

struct Mesh
{
    RenderType type;
    std::vector<Triangle> triangles;
};

struct Camera
{
    int cameraId;
    ProjectionType projectionType;
    vec4 pos;
    vec4 gaze;
    vec4 u;
    vec4 v;
    vec4 w;
    double left, right, bottom, top;
    double near;
    double far;
    int horRes;
    int verRes;
    std::string outputFileName;
};

#endif