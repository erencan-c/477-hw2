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
    ProjectionType projection_type;
    vec4 pos;
    vec4 gaze;
    vec4 u;
    vec4 v;
    vec4 w;
    double left, right, bottom, top;
    double near;
    double far;
    int width;
    int height;
    std::string output_file_name;
};

#endif