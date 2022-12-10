#ifndef __MESH_H__
#define __MESH_H__

#include <vector>
#include "Triangle.h"
#include <iostream>

using namespace std;

class Mesh
{

public:
    int id;
    int type; // 0 for wireframe, 1 for solid
    vector<Triangle> triangles;

    Mesh();

    friend ostream &operator<<(ostream &os, const Mesh &m);
};

#endif