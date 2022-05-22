#pragma once

#include <string>

// template<typename _t> struct vec1_like { _t x; };
// template<typename _t> struct vec2_like { _t x, y; };
// template<typename _t> struct vec3_like { _t x, y, z; };
// template<typename _t> struct vec4_like { _t x, y, z, w; };

// using vec1b = vec1_like<bool>;
// using vec2b = vec2_like<bool>;
// using vec3b = vec3_like<bool>;
// using vec4b = vec4_like<bool>;

// using vec1i = vec1_like<int>;
// using vec2i = vec2_like<int>;
// using vec3i = vec3_like<int>;
// using vec4i = vec4_like<int>;

// using vec1 = vec1_like<float>;
// using vec2 = vec2_like<float>;
// using vec3 = vec3_like<float>;
// using vec4 = vec4_like<float>;

// using quat = vec4;

struct NameComponent
{
	std::string Name;
};

struct Transform
{
	vec3 Position;
	vec3 Rotation;
	vec3 Scale;
};