#pragma once

#include <tuple>

inline std::tuple<float, float, float, float> GetShot(
	float x,  float y,
	float tx, float ty,
	float speed, float margin)
{
	float dx = tx - x,
	      dy = ty - y;

	float length = sqrt(dx*dx + dy*dy);

	float nx = dx / length,
           ny = dy / length;

	dx *= speed / length;
	dy *= speed / length;

	x += nx * margin;
	y += ny * margin;

	return std::tie(x, y, dx, dy);
}
