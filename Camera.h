#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "vec.hpp"
#include <string>

using namespace std;

class Camera
{

public:
    int cameraId;
    int projectionType; // 1 for perspective, 0 for orthographic
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
    string outputFileName;

    Camera();

    Camera(int cameraId,
           int projectionType,
           vec4 pos, vec4 gaze,
           vec4 u, vec4 v, vec4 w,
           double left, double right, double bottom, double top,
           double near, double far,
           int horRes, int verRes,
           string outputFileName);

    Camera(const Camera &other);

    friend std::ostream &operator<<(std::ostream &os, const Camera &c);
};

#endif