#pragma once

#include <vector>
//#include "iw/math/vector2.h"
#include "glm/vec2.hpp"
#include <algorithm>

namespace iw {
namespace common {
	template<
		typename _t>
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		_t* field,
		size_t width, size_t height,
		const _t& threshhold,
		std::function<bool(const _t&, const _t&)> test = [](const _t& a,
												  const _t& b) { return a >= b; })
	{
		glm::vec2 midPoints[4] = {
			glm::vec2(0.0, -0.5),   //    1
			glm::vec2(0.5, -1.0),   //  0   3
			glm::vec2(0.5, -0.0),   //    2
			glm::vec2(1.0, -0.5), 
		};

		int _ = -1;
		int edgeIndices[16][4] = {
			{ _, _, _, _ },
			{ 2, 3, _, _ }, // /
			{ 1, 3, _, _ }, // \ 
			{ 1, 2, _, _ }, // |
			{ 0, 2, _, _ }, // \  
			{ 0, 3, _, _ }, // -- 
			{ 0, 2, 1, 3 }, // \\ 
			{ 0, 1, _, _ }, // / 
			{ 0, 1, _, _ }, // /
			{ 0, 1, 2, 3 }, // // 
			{ 0, 3, _, _ }, // --
			{ 0, 2, _, _ }, // \ 
			{ 1, 2, _, _ }, // | 
			{ 1, 3, _, _ }, // \ 
			{ 2, 3, _, _ }, // /
			{ _, _, _, _ }
		};

		size_t iHeight = 2 * height - 1; // size of index (2 stripes)

		//  a-3-c---•---x
		//  0   7   |   |
		//  b-4-d---•---x
		//  1   8   |   |
		//  •-5-•---•---x
		//  2   9   |   |
		//  x-6-x---x---x

		std::unordered_map<size_t, glm::vec2> verts;
		std::vector<size_t> edges;

		for (size_t i = 0; i < width  - 1; i++)
		for (size_t j = 0; j < height - 1; j++)
		{
			size_t a =  i      +  j      * width;
			size_t b =  i      + (j + 1) * width;
			size_t c = (i + 1) +  j      * width;
			size_t d = (i + 1) + (j + 1) * width;

			// abcd = 0b0000

			bool left   = i == 0;
			bool top    = j == 0;
			bool right  = i == width  - 2; // -2 <-> loop ends at -1
			bool bottom = j == height - 2;

			bool lt = left  || top;    // to address edge case on boundary, treat any point on bound as empty
			bool lb = left  || bottom;
			bool rt = right || top;
			bool rb = right || bottom;

			int state = int( (!lt) & test(field[a], threshhold) ) * 8 
					+ int( (!lb) & test(field[b], threshhold) ) * 4
					+ int( (!rt) & test(field[c], threshhold) ) * 2
					+ int( (!rb) & test(field[d], threshhold) );

			for (size_t k = 0; k < 4; k++)
			{
				int edgeIndex = edgeIndices[state][k];
				if (edgeIndex == _) break; // was contiune

				int index = (2 * height - 1) * i;

				switch (edgeIndex)
				{
					case 0: index += j;              break;
					case 1: index += j + height - 1; break;
					case 2: index += j + height;     break;
					case 3: index += j + iHeight;    break;
				}

				edges.push_back(index);

				if (verts.find(index) == verts.end()) {
					verts.emplace(index, midPoints[edgeIndex] + glm::vec2(i, j));
				}
			}
		}

#ifdef IW_DEBUG
		std::unordered_map<size_t, int> map;
		for (auto i : edges) map[i]++;
		for (auto [i, c] : map) assert(c == 2);
#endif 

		// Find the chains of edges

		std::vector<std::vector<size_t>> chains;

		while (edges.size() > 0)
		{
			std::vector<size_t> chain = {
				edges[0],
				edges[1]
			};

			edges.erase(edges.begin()); // yuk
			edges.erase(edges.begin());

			while (chain.front() != chain.back())
			{
				size_t index = std::distance(edges.begin(), std::find(edges.begin(), edges.end(), chain.back()));
				size_t other = index % 2 == 0 ? index + 1 : index - 1;

				chain.push_back(edges.at(other));

				edges.erase(edges.begin() + (index > other ? index : other)); // erase verts from list, last first to keep index intact
				edges.erase(edges.begin() + (index > other ? other : index));
			}

			chains.emplace_back(chain);
		}

		if (chains.size() == 0) return {};

		// Remove edges on straights

		for (auto& chain : chains)
		{
			size_t last = 0;
			size_t lastDif = 0;

			for (size_t i = 2; i <= chain.size(); i++)
			{
				double dif = llabs(chain[i - 2] - chain[i % chain.size()]) / 2.0;

				if (    dif == 1           // |
					|| dif == iHeight     // --
					|| dif == height      // \ 
					|| dif == height - 1) // /    
				{
					continue;
				}

				else if (i - last - 2 > 2)
				{
					chain.erase(chain.begin() + last + 1, chain.begin() + i - 1);
					i = last + 1;
				}

				last = i - 1;
			}
		}

		// Get verts from chains

		std::vector<std::vector<glm::vec2>> polygons;

		for (auto& chain : chains)
		{
			chain.pop_back();

			std::vector<glm::vec2> polygon;
			for (size_t& index : chain) {
				polygon.push_back(verts.at(index));
			}

			// Connect squares corners

			for (size_t i = 0; i < polygon.size(); i++) {
				size_t j = (i + 1) % polygon.size();
				size_t k = (i + 2) % polygon.size();
				size_t l = (i + 3) % polygon.size();

				glm::vec2& v1 = polygon[j];
				glm::vec2& v2 = polygon[k];

				glm::vec2 v12 = v2 - v1;

				if (v12.x == 0 || v12.y == 0) continue;

				glm::vec2 vij = polygon[i] - polygon[j];
				glm::vec2 vkl = polygon[k] - polygon[l];

				if (glm::dot(vij, vkl) == 0) {

					glm::vec2 v(0.f);

					if (    (v12.x > 0 && v12.y > 0)
						|| (v12.x < 0 && v12.y < 0))
					{
						v = glm::vec2(v2.x, v1.y);
					}

					if (    (v12.x > 0 && v12.y < 0)
						|| (v12.x < 0 && v12.y > 0))
					{
						v = glm::vec2(v1.x, v2.y);
					}

					if (v == glm::vec2(0)) continue;

					polygon.erase(polygon.begin() + k);
					polygon[j] = v;
				}
			}

			polygons.emplace_back(polygon);
		}

		return polygons;
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
					if (ia == j || ib == j || ic == j) continue;
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

	using namespace common;
}
