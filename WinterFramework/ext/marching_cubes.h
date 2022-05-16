#pragma once

#include "hitbox/Hitbox.h"

#include <vector>
#include <functional>
#include <stdio.h>

template<typename _t>
std::pair<std::vector<std::vector<glm::vec2>>, HitboxBounds>
MakePolygonFromField(
	_t* field,
	size_t width, size_t height,
	const _t& threshold)
{
	return MakePolygonFromField(
		field, width, height,
		[&](const _t& x) { return x >= threshold; }
	);
}

template<typename _t>
std::pair<std::vector<std::vector<glm::vec2>>, HitboxBounds>
MakePolygonFromField(
	_t* field,
	size_t width, size_t height,
	std::function<bool(const _t&)> test)
{
	bool* f = new bool[width * height];
	for (size_t i = 0; i < width * height; i++) f[i] = test(field[i]);
	auto out = make_hitbox(f, width, height, 80); // 80 gives best values for the station enemy, everything else looks ok too
	delete[] f;
	return out;
}