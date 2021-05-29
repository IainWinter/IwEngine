#pragma once

#include <vector>
//#include "iw/math/vector2.h"
#include "glm/vec2.hpp"
#include <algorithm>

namespace iw {
namespace common {
	template<
		typename _t>
	std::vector<glm::vec2> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		_t threshhold)
	{
		std::vector<glm::vec2> edges;

		glm::vec2 midPoints[4] = {
			glm::vec2(0.5, 0),
			glm::vec2(1,   0.5),
			glm::vec2(0.5, 1),
			glm::vec2(0,   0.5),
		};

		int _ = -1;
		int edgeIndices[16][4] = {
			{ _, _, _, _ },
			{ 2, 3, _, _ },
			{ 1, 2, _, _ },
			{ 1, 3, _, _ },
			{ 0, 1, _, _ },
			{ 0, 3, 1, 2 },
			{ 0, 2, _, _ },
			{ 0, 3, _, _ },
			{ 0, 3, _, _ },
			{ 0, 2, _, _ },
			{ 0, 1, 2, 3 },
			{ 0, 1, _, _ },
			{ 1, 3, _, _ },
			{ 1, 2, _, _ },
			{ 2, 3, _, _ },
			{ _, _, _, _ }
		};

		for (size_t i = 0; i < width  - 1; i++)
		for (size_t j = 0; j < height - 1; j++)
		{
			//  a---b---•---x
			//  |   |   |   |
			//  d---c---•---x
			//  |   |   |   |
			//  •---•---•---x
			//  |   |   |   |
			//  x---x---x---x

			size_t a = i + j * width;
			size_t b = i + j * width + 1;
			size_t c = i + j * width + 1 + width;
			size_t d = i + j * width     + width;

			// abcd = 0b0000

			int state = int(field[a] > threshhold) * 8
					+ int(field[b] > threshhold) * 4
					+ int(field[c] > threshhold) * 2
					+ int(field[d] > threshhold);

			for (size_t k = 0; k < 4; k++) {
				int edgeIndex = edgeIndices[state][k];
				if (edgeIndex == _) continue;

				glm::vec2 vert = midPoints[edgeIndex];
				vert.x += i;
				vert.y += j;

				edges.push_back(vert);
			}
		}

		if (edges.size() == 0) return {};

		std::vector<glm::vec2> chain = {
			edges[0],
			edges[1]
		};

		edges.erase(edges.begin());
		edges.erase(edges.begin());

		while (edges.size() > 0) {
			size_t index = std::distance(edges.begin(), std::find(edges.begin(), edges.end(), chain.back()));
			size_t other = index % 2 == 0 ? index + 1 : index - 1;

			chain.push_back(edges.at(other));

			edges.erase(edges.begin() + (index > other ? index : other)); // erase verts from list, last first to keep index intact
			edges.erase(edges.begin() + (index > other ? other : index));
		}

		chain.pop_back();

		for (size_t i = 1; i < chain.size() - 1; i++) {
			if (   (chain.at(i).x == chain.at(i + 1).x
				|| chain.at(i).y == chain.at(i + 1).y)
				&&(chain.at(i).x == chain.at(i - 1).x
				|| chain.at(i).y == chain.at(i - 1).y))
			{
				chain.erase(chain.begin() + i);
				i--;
			}
		}

		std::reverse(chain.begin(), chain.end());

		return chain;
	}

	inline float cross_length(glm::vec2 a, glm::vec2 b) {
		return a.x * b.y - a.y * b.x;
	}

		// put these in cpp

	// true = clockwise, false = counter-clockwise
	inline bool IsClockwise(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
		return cross_length(b - a, c - a) < 0; // this was backwards before idk how that worked?
	}

	inline bool HasPoint(glm::vec2 a, glm::vec2 b, glm::vec2 c, glm::vec2 p) {
		if ((a.x - b.x) / (a.y - b.y) == (a.x - c.x) / (a.y - c.y)) {
			return true; // points are on a line
		}

		glm::vec2 v  = p;
		glm::vec2 v0 = a;
		glm::vec2 v1 = b - a;
		glm::vec2 v2 = c - a;

		float ca =  (cross_length(v, v2) - cross_length(v0, v2)) / cross_length(v1, v2);
		float cb = -(cross_length(v, v1) - cross_length(v0, v1)) / cross_length(v1, v2);

		return ca > 0 && cb > 0 && ca + cb < 1;
	}

	inline std::vector<unsigned> TriangulatePolygon(
		const std::vector<glm::vec2>& polygon)
	{
		if (polygon.size() == 0) return {};

		std::vector<unsigned> triangles;

		std::vector<std::pair<glm::vec2, size_t>> working; // copy a version with the correct indices
		for (size_t i = 0; i < polygon.size(); i++) {
			working.emplace_back(polygon.at(i), i);
		}

		while (true)
		{
			int workingSize = working.size();

			for (size_t i = 0; i < working.size(); i++)
			{
				if (working.size() < 3) { // exit
					goto exit;
				}

				size_t ia = (i    );
				size_t ib = (i + 1) % working.size();
				size_t ic = (i + 2) % working.size();

				auto [a, ria] = working.at(ia);
				auto [b, rib] = working.at(ib);
				auto [c, ric] = working.at(ic);

				if (IsClockwise(a, b, c)) continue; // Invalid triangle if clockwise winding (should find order of polygon, above function always gives CC I'm 70% sure)

				bool hasPoint = false;

				for (size_t j = 0; j < working.size(); j++) {
					//if (ia == j || ib == j || ic == j) continue;
					if (hasPoint |= HasPoint(a, b, c, working.at(j).first)) break;
				}

				if (hasPoint) continue;

				triangles.push_back(ria); // size_t -> unsigned int
				triangles.push_back(rib);
				triangles.push_back(ric);

				working.erase(working.begin() + ib);
			}

			if (workingSize == working.size()) {
				assert(false);
				break;
			}
		}

	exit:
		return triangles;
	}
}
}
