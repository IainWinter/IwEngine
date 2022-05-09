#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using u32 = uint32_t;
using u8 = uint8_t;

struct Color
{
	union { u32 as_u32; struct { u8 r, g, b, a; }; };
	Color(u8 r, u8 g, u8 b, u8 a = 255) : r(r), g(g), b(b), a(a) {}
	Color() : r(255), g(255), b(255), a(255) {}

	float rf() const { return r / (float)255.f; }
	float gf() const { return g / (float)255.f; }
	float bf() const { return b / (float)255.f; }
	float af() const { return a / (float)255.f; }

	glm::vec4 as_v4() const
	{
		return { rf(), gf(), bf(), af() };
	}
};

struct Transform2D
{
	float x, y, z, r, sx, sy;

	glm::mat4 World() const
	{
		glm::mat4 world = glm::mat4(1.f);
		world = glm::rotate(world, r, glm::vec3(0, 0, 1.f));
		world = glm::scale(world, glm::vec3(sx, sy, 1.f));
		world = glm::translate(world, glm::vec3(x, y, z));

		return world;
	}
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