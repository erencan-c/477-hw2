#include "line.hpp"

vec4 lerp_color(float cur, float min, float max, vec4 start, vec4 end)
{
	float alpha = (cur - min) / (max - min);
	return (1 - alpha) * start + alpha * end;
}

void draw_line(int x1, int y1, int x2, int y2, vec4 start_color, vec4 end_color, vec4** image_buffer, const int width, const int height)
{
	if (x1 > x2)
	{
		draw_line(x2, y2, x1, y1, end_color, start_color, image_buffer, width, height);
		return;
	}

	int draw_test = 0;
	int m = 0;
	int dx = x2 - x1;
	int dy = y2 - y1;
	int d = dx - 2 * dy;
	int new_d = dy - 2 * dx;
	int y = y1;
	int x = x1;
	float line_slope = (float)dy / dx;

	if (dy < 0)
	{
		m = -1;
		dy = -dy;
	}
	else if (dy >= 0)
	{
		m = 1;
	}

	if (line_slope > -1.0 && line_slope <= 1.0 && dx != 0)
	{
		for (x = x1; x < x2; x++)
		{
			if (x >= 0)
				image_buffer[x][y] = lerp_color(x, x1, x2, start_color, end_color);

			if (d <= 0)
			{
				y += m;
				d += 2 * (dx - dy);
			}
			else
			{
				d += -2 * dy;
			}
		}

	}
	else
	{
		if (y1 > y2)
		{
			x = x2;
			std::swap(start_color, end_color);
			std::swap(y1, y2);
		}

		for (y = y1; y < y2; y++)
		{
			if (x >= 0)
				image_buffer[x][y] = lerp_color(y, y1, y2, start_color, end_color);

			if (new_d <= 0)
			{
				x += m;
				new_d += 2 * (dy - dx);
			}
			else
			{
				new_d += -2 * dx;
			}
		}

	}
}  

float maxi(float arr[], int n)
{
	float m = 0;
	for (int i = 0; i < n; ++i)
		if (m < arr[i])
			m = arr[i];
	return m;
}

float mini(float arr[], int n)
{
	float m = 1;
	for (int i = 0; i < n; ++i)
		if (m > arr[i])
			m = arr[i];
	return m;
}

void clip_line(float x1, float y1, float x2, float y2, vec4 start_color, vec4 end_color, vec4** image_buffer, int width, int height)
{
	float p1 = -(x2 - x1);
	float p2 = -p1;
	float p3 = -(y2 - y1);
	float p4 = -p3;

	float q1 = x1;
	float q2 = width - x1 - 1;
	float q3 = y1;
	float q4 = height - y1 - 1;

	float posarr[5], negarr[5];
	int posind = 1, negind = 1;
	posarr[0] = 1;
	negarr[0] = 0;

	if ((p1 == 0 && q1 < 0) || (p2 == 0 && q2 < 0) || (p3 == 0 && q3 < 0) || (p4 == 0 && q4 < 0))
	{
		return;
	}
	if (p1 != 0)
	{
		float r1 = q1 / p1;
		float r2 = q2 / p2;
		if (p1 < 0)
		{
			negarr[negind++] = r1;
			posarr[posind++] = r2;
		}
		else
		{
			negarr[negind++] = r2;
			posarr[posind++] = r1;
		}
	}
	if (p3 != 0)
	{
		float r3 = q3 / p3;
		float r4 = q4 / p4;
		if (p3 < 0)
		{
			negarr[negind++] = r3;
			posarr[posind++] = r4;
		}
		else
		{
			negarr[negind++] = r4;
			posarr[posind++] = r3;
		}
	}

	float xn1, yn1, xn2, yn2;
	float rn1, rn2;
	rn1 = maxi(negarr, negind);
	rn2 = mini(posarr, posind);

	//outside window
	if (rn1 > rn2)
		return;

	xn1 = x1 + p2 * rn1;
	yn1 = y1 + p4 * rn1;

	xn2 = x1 + p2 * rn2;
	yn2 = y1 + p4 * rn2;

	//TODO fix color interpolation here (parts of the line may not be drawn yet color starts at start_color)
	auto fixed_start_color = lerp_color(xn1, x1, x2, start_color, end_color);
	auto fixed_end_color = lerp_color(xn2, x1, x2, start_color, end_color);
	draw_line(xn1, yn1, xn2, yn2, fixed_start_color, fixed_end_color, image_buffer, width, height);
}