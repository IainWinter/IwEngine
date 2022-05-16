#pragma once

#include "glm/vec2.hpp"
#include "glm/common.hpp"
#include "glm/geometric.hpp"

using namespace glm; // going to start doing this as glm should just be the default math lib, no need to namespace everywhere

#include <utility>
#include <array>
#include <vector>
#include <float.h>

struct HitboxBounds
{
	vec2 Min;
	vec2 Max;

	HitboxBounds()
	{
		Min = vec2(-FLT_MAX, -FLT_MAX);
		Max = vec2( FLT_MAX,  FLT_MAX);
	}

	float Width()  const { return Max.x - Min.x; }
	float Height() const { return Max.y - Min.y; }
};

bool is_polygon_convex(const std::vector<vec2>& polygon);
HitboxBounds make_bounds(const std::vector<vec2>& points);

std::vector<vec2> make_contour(const bool* mask_grid, int width, int height);
std::vector<vec2> make_polygon(const std::vector<vec2>& contour, const HitboxBounds& boundingBox, int accuracy);
std::pair<std::vector<std::vector<vec2>>, HitboxBounds> make_hitbox(const bool* mask_grid, int width, int height, int accuracy);