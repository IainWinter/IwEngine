#pragma once

#include <vector>
#include "iw/math/vector2.h"

namespace iw {
namespace common {
	template<
		typename _t>
	std::vector<iw::vector2> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		_t threshhold)
	{
		std::vector<iw::vector2> edges;

		iw::vector2 midPoints[4] = {
			iw::vector2(0.5, 0),
			iw::vector2(1,   0.5),
			iw::vector2(0.5, 1),
			iw::vector2(0,   0.5),
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

				iw::vector2 vert = midPoints[edgeIndex];
				vert.x += i;
				vert.y += j;

				edges.push_back(vert);
			}
		}

		std::vector<iw::vector2> chain = {
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

		return chain;
	}

		// put these in cpp

	// true = clockwise, false = counter-clockwise
	inline bool IsClockwise(iw::vector2 a, iw::vector2 b, iw::vector2 c) {
		return (b - a).cross_length(c - a) > 0;
	}

	inline bool HasPoint(iw::vector2 a, iw::vector2 b, iw::vector2 c, iw::vector2 p) {
		if ((a.x - b.x) / (a.y - b.y) == (a.x - c.x) / (a.y - c.y)) {
			return true; // points are on a line
		}

		iw::vector2 v  = p;
		iw::vector2 v0 = a;
		iw::vector2 v1 = b - a;
		iw::vector2 v2 = c - a;

		float ca =  (v.cross_length(v2) - v0.cross_length(v2)) / v1.cross_length(v2);
		float cb = -(v.cross_length(v1) - v0.cross_length(v1)) / v1.cross_length(v2);

		return ca > 0 && cb > 0 && ca + cb < 1;
	}

	inline std::vector<unsigned> TriangulatePolygon(
		const std::vector<iw::vector2>& polygon)
	{
		std::vector<unsigned> triangles;

		std::vector<std::pair<iw::vector2, size_t>> working; // copy a version with the correct indices
		for (size_t i = 0; i < polygon.size(); i++) {
			working.emplace_back(polygon.at(i), i);

			LOG_INFO << polygon.at(i);
		}

		while (working.size() >/*=*/ 3) {
			for (size_t i = 0; i < working.size(); i++) {
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
		}

		return triangles;
	}
}
}
