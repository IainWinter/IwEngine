#pragma once

#include "iw/common/algos/geom2.h"

#include <vector>
#include <algorithm>
#include <functional>

// should split and make cpp file

namespace iw {
namespace common {
	using polygon_cut = std::tuple<
		std::vector<glm::vec2>, std::vector<unsigned>,
		std::vector<glm::vec2>, std::vector<unsigned>>;

	template<
		typename _t>
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		const _t& threshhold,
		std::function<bool(const _t&, const _t&)> test = [](const _t& a,
												  const _t& b) { return a >= b; })
	{
		bool* f = new bool[width * height];

		for (size_t i = 0; i < width * height; i++)
		{
			f[i] = test(field[i], threshhold);
		}

		auto out = MakePolygonFromField(f, width, height);

		delete[] f;

		return out;
	}

	IWCOMMON_API
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		bool* field,
		size_t width, size_t height);

	IWCOMMON_API
	std::vector<unsigned> TriangulatePolygon(
		const std::vector<glm::vec2>& polygon);

	IWCOMMON_API
	polygon_cut CutPolygon(
		const std::vector<glm::vec2>& verts,
		const std::vector<unsigned>& index,
		glm::vec2 lineA, glm::vec2 lineB);

	IWCOMMON_API
	void TransformPolygon(
		std::vector<glm::vec2>& polygon,
		const iw::Transform* transform);

	IWCOMMON_API
	void AddPointToPolygon(
		std::vector<glm::vec2>& verts,
		std::vector<unsigned>& indices,
		glm::vec2 point);
}

	using namespace common;
}
