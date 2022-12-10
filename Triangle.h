#ifndef __TRIANGLE_H__
#define __TRIANGLE_H__

#include "vec.hpp"

struct Triangle {
    vec4 vertices[3];
    vec4 colors[3];
};

#endif