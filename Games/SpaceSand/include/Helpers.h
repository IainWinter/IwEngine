#pragma once

#include <tuple>

inline std::tuple<float, float, float, float> GetShot(
	float x,  float y,
	float tx, float ty,
	float speed, float margin, float thickness = 0)
{
	float dx = tx - x,
	      dy = ty - y;

	float length = sqrt(dx*dx + dy*dy);

	float nx = dx / length,
           ny = dy / length;

	dx = nx * speed;
	dy = ny * speed;

	float r = iw::randfs();

	x += nx * margin - ny * thickness * r;
	y += ny * margin + nx * thickness * r;

	return std::tie(x, y, dx, dy);
}
