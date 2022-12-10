#include <vector>
#include "Triangle.h"
#include "Mesh.h"
#include <iostream>
#include <iomanip>

using namespace std;

Mesh::Mesh() {}

ostream &operator<<(ostream &os, const Mesh &m)
{
    os << "Mesh " << m.id;

    if (m.type == 0)
    {
        os << " wireframe(0) with ";
    }
    else
    {
        os << " solid(1) with ";
    }

    os << fixed << setprecision(3) << m.triangles.size() << " triangles"
       << endl << "\tTriangles are:" << endl << fixed << setprecision(0);

    for (int i = 0; i < m.triangles.size(); i++) {
        os << "\t\t" << m.triangles[i].vertices[0] << " " << m.triangles[i].vertices[1] << " " << m.triangles[i].vertices[2] << endl;
    }

    return os;
}
