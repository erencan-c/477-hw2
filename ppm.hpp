#ifndef __PPM_H__
#define __PPM_H__

#include "vec.hpp"

void write_ppm(vec4** image_buffer, int width, int height, std::string filename);

#endif