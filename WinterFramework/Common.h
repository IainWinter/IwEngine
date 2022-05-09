#pragma once

struct Transform2D
{
	float x, y, r;
};

float lerp(float a, float b, float w)
{
	return a + w * (b - a);
}

float clamp(float x, float min, float max)
{
	     if (x < min) x = min;
	else if (x > max) x = max;
	return x; 
}