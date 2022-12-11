#ifndef __LINE_H__
#define __LINE_H__

#include "vec.hpp"

void clip_line(float x1, float y1, float x2, float y2, vec4 start_color, vec4 end_color, vec4** image_buffer, int width, int height);

#endif