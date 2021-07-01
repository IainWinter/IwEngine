#include "iw/common/algos/polygon2.h"

#include <vector>
#include <algorithm>
#include <functional>
#include <set>

// should split and make cpp file

namespace iw {
namespace common {
	std::vector<std::vector<glm::vec2>> MakePolygonFromField(
		bool* field,
		size_t width, size_t height)
	{
		if (width <= 2 || height <= 2) return {}; // to protect width/height - 1 in for loops

		glm::vec2 midPoints[4] = {
			glm::vec2(0.0, 0.5),   //    1
			glm::vec2(0.5, 1.0),   //  0   3
			glm::vec2(0.5, 0.0),   //    2
			glm::vec2(1.0, 0.5), 
		};

		// needs reversed y for correct location? - (solution -> the iteration is backwards on y)

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

		//  x-6-x---x---x
		//  2   9   |   |
		//  •-5-•---•---x
		//  1   8   |   |
		//  b-4-c---•---x
		//  0   7   |   |
		//  a-3-d---•---x

		std::unordered_map<size_t, glm::vec2> verts;
		std::vector<size_t> edges;

		for (size_t i = 0; i < width  - 1; i++)
		for (size_t j = 0; j < height - 1; j++)
		{
			size_t a =  i      + (j + 1) * width;
			size_t b =  i      +  j      * width;
			size_t c = (i + 1) + (j + 1) * width;
			size_t d = (i + 1) +  j      * width;
			
			// abcd = 0b0000

			bool left   = i == 0;
			bool bottom = j == 0;
			bool right  = i == width  - 2; // -2 <-> loop ends at -1
			bool top    = j == height - 2;

			bool lt = left  || top;    // to address edge case on boundary, treat any point on bound as empty
			bool lb = left  || bottom;
			bool rt = right || top;
			bool rb = right || bottom;

			int state = int( (!lt) & field[a] ) * 8 
					  + int( (!lb) & field[b] ) * 4
					  + int( (!rt) & field[c] ) * 2
					  + int( (!rb) & field[d] );

			for (size_t k = 0; k < 4; k++)
			{
				int edgeIndex = edgeIndices[state][k];
				if (edgeIndex == _) break; // was contiune

				int index = (2 * height - 1) * i;

				switch (edgeIndex)
				{
					case 0: index += j;              break;
					case 1: index += j + height;     break;
					case 2: index += j + height - 1; break;
					case 3: index += j + iHeight;    break;
				}

				edges.push_back(index);

				if (verts.find(index) == verts.end())
				{
					glm::vec2 v = midPoints[edgeIndex] + glm::vec2(i, j);

					     if (left)   v.x -= 0.5f; // location edge case fix
					else if (right)  v.x += 0.5f;

						if (top)    v.y += 0.5f;
					else if (bottom) v.y -= 0.5f;

					verts.emplace(index, v);
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

				else if (i - last - 1 > 2) // was '- 2' check which is correct or if there are further edge cases?
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

			if (true) // todo: doesnt work in some cases!!!! (triangles are wrong with >>   //______
			for (size_t i = 0; i < polygon.size(); i++) {							 //|   x|
				size_t j = (i + 1) % polygon.size();								 //|  xx|
				size_t k = (i + 2) % polygon.size();								 //| xxx|
				size_t l = (i + 3) % polygon.size();								 //|____|

				glm::vec2& v1 = polygon[j];
				glm::vec2& v2 = polygon[k];

				glm::vec2 v12 = v2 - v1;

				if (v12.x == 0 || v12.y == 0) continue;

				glm::vec2 vij = polygon[j] - polygon[i];
				glm::vec2 vkl = polygon[l] - polygon[k];

				if (glm::dot(vij, vkl) == 0)
				{
					glm::vec2 v;

					if (   (v12.x > 0 && v12.y > 0)
						|| (v12.x < 0 && v12.y < 0))
					{
						v = glm::vec2(v2.x, v1.y);
					}

					else 
					if (   (v12.x > 0 && v12.y < 0)
						|| (v12.x < 0 && v12.y > 0))
					{
						v = glm::vec2(v1.x, v2.y);
					}

					else continue;

					assert(j < polygon.size());

					polygon.erase(polygon.begin() + k);
					polygon[j] = v;
				}
			}

			for (glm::vec2& vert : polygon) { // round to corect scale
				vert = glm::round(vert);
			}

			polygons.emplace_back(polygon);
		}

		return polygons;
	}

	std::vector<unsigned> TriangulatePolygon(
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
				//assert(false);
				break;
			}
		}

	exit:

		//for ()
	// edge cases fail sometimes (overlapping tris)
	//std::vector<unsigned> TriangulateSweep(
	//	std::vector<glm::vec2>& verts)
	//{
	//	std::sort(verts.begin(), verts.end(), [](glm::vec2& a, glm::vec2& b)
	//	{ 
	//		return a.x == b.x ? a.y < b.y : a.x < b.x; 
	//	});

	//	std::vector<unsigned> index = { 0, 1, 2 };
  //     pointList is a set of coordinates defining the points to be triangulated
	//	auto append = [&](unsigned a, unsigned b, unsigned c) {
	//		index.push_back(a);
	//		index.push_back(b);
	//		index.push_back(c);
	//	};

	//	for (size_t i = 3; i < verts.size(); i++)
	//	{
	//		const glm::vec2& a = verts[index[index.size() - 3]];
	//		const glm::vec2& b = verts[index[index.size() - 2]];
	//		const glm::vec2& c = verts[index[index.size() - 1]];
	//		const glm::vec2& d = verts[i];

	//		glm::vec2 ac = c - a;
	//		glm::vec2 normal = glm::vec2(-ac.y, ac.x);

	//		bool DaboveC  = d.y > c.y;
	//		bool BaboveAC = glm::dot(b - a, normal) > 0;

	//		append(i - (DaboveC ^ BaboveAC ? 3 : 2), i - 1, i);
	//	}

	//	// Sorting the points -> non CC, this fixes them

	//	for (size_t i = 0; i < index.size(); i += 3)
	//	{
	//		auto& [a, b, c] = GetTriangle(verts, index, i);
	//		if (IsClockwise(a, b, c)) {
	//			std::swap(index[i], index[i + 1]);
	//		}
	//	}

	//	return index;
	//}

  //             add triangle to badTriangles
					badTriangles.push_back(i);
				}
			}

  //       polygon := empty set
			std::set<std::pair<unsigned, unsigned>> edges;

  //       for each triangle in badTriangles do // find the boundary of the polygonal hole
			for (unsigned triangle : badTriangles)
			{
  //          for each edge in triangle do
				for (size_t e = 0; e < 3; e++)
				{
					unsigned e1 = index[triangle +  e];
					unsigned e2 = index[triangle + (e + 1) % 3];
					if (e1 > e2) std::swap(e1, e2);

  //             if edge is not shared by any other triangles in badTriangles
					if (edges.find({ e1, e2 }) == edges.end())
					{
  //                add edge to polygon
						edges.emplace(e1, e2);
					}
				}
			}

  //       for each triangle in badTriangles do // remove them from the data structure
			for (auto t = badTriangles.rbegin(); t != badTriangles.rend(); t++)
			{
  //          remove triangle from triangulation
				index.erase(index.begin() + *t + 2);
				index.erase(index.begin() + *t + 1);
				index.erase(index.begin() + *t);
			}

  //       for each edge in polygon do // re-triangulate the polygonal hole
			for (std::pair<unsigned, unsigned> edge : edges) {
  //          newTri := form a triangle from edge to point
  //          add newTri to triangulation
				AddPointToPolygon(working, index, working[edge.first]);
				AddPointToPolygon(working, index, working[edge.second]);
				AddPointToPolygon(working, index, v);
			}
		}

		return { working, index };

  //    for each triangle in triangulation // done inserting points, now clean up
  //       if triangle contains a vertex from original super-triangle
  //          remove triangle from triangulation
  //    return triangulation
	}

	void RemoveTinyTriangles(
		std::vector<glm::vec2>& polygon, 
		std::vector<unsigned>& index, 
		float raiotOfAreaToRemove)
	{
		float totalArea = 0;

		for (size_t i = 0; i < index.size(); i += 3)
		{
			auto& [a, b, c] = GetTriangle(polygon, index, i);
			totalArea += TriangleArea(a, b, c);
		}

		for (size_t i = 0; i < index.size(); i += 3)
		{
			auto& [a, b, c] = GetTriangle(polygon, index, i);
			float area = TriangleArea(a, b, c);

			if (area / totalArea < 0.01) {
				index.erase(index.begin() + i + 2);
				index.erase(index.begin() + i + 1);
				index.erase(index.begin() + i);
				i -= 3;
			}
		}

		// could remove from polygons too but need to subtract from index after removed
		
		//std::set<unsigned> indexBefore(index.begin(), index.end()); // was at the top of function

		//for (auto itr = indexBefore.rbegin(); itr != indexBefore.rend(); itr++)
		//{
		//	unsigned find = *itr;
		//	bool found = std::find(index.begin(), index.end(), find) == index.end();

		//	if (found)
		//	{
		//		polygon.erase(polygon.begin() + *itr);
		//	}
		//}
	}

	polygon_cut CutPolygon(
		const std::vector<glm::vec2>& verts,
		const std::vector<unsigned>& index,
		glm::vec2 lineA, glm::vec2 lineB) 
	{
		std::vector<glm::vec2> clVerts;
		std::vector<glm::vec2> crVerts;
		std::vector<unsigned> clIndex;
		std::vector<unsigned> crIndex;

		glm::vec2 line = lineB - lineA;
		glm::vec2 tangent = glm::vec2(-line.y, line.x);

		float lineLength = glm::length(line);

		for (size_t i = 0; i < index.size(); i += 3)
		{
			glm::vec2 a = verts[index[i    ]];
			glm::vec2 b = verts[index[i + 1]];
			glm::vec2 c = verts[index[i + 2]];

			bool aRight = glm::dot(a - lineA, tangent) > 0; // could make "SamdDirection" a common function
			bool bRight = glm::dot(b - lineA, tangent) > 0;
			bool cRight = glm::dot(c - lineA, tangent) > 0;

			if (aRight && bRight && cRight)
			{
				AddPointToPolygon(crVerts, crIndex, a);
				AddPointToPolygon(crVerts, crIndex, b);
				AddPointToPolygon(crVerts, crIndex, c);
			}

			else if (!aRight && !bRight && !cRight)
			{
				AddPointToPolygon(clVerts, clIndex, a);
				AddPointToPolygon(clVerts, clIndex, b);
				AddPointToPolygon(clVerts, clIndex, c);
			}

			else 
			{
				unsigned singlePoint = 0;
				if (aRight == cRight) singlePoint = 1;
				if (aRight == bRight) singlePoint = 2;

				unsigned i0 = i +  singlePoint;
				unsigned i1 = i + (singlePoint + 1) % 3;
				unsigned i2 = i + (singlePoint + 2) % 3;

				glm::vec2 v0 = verts[index[i0]];
				glm::vec2 v1 = verts[index[i1]];
				glm::vec2 v2 = verts[index[i2]];

				glm::vec2 v01 = LineIntersection(v0, v1, lineA, lineB);
				glm::vec2 v02 = LineIntersection(v0, v2, lineA, lineB);

				bool singleRight = glm::dot(v0 - lineA, tangent) > 0;

				std::vector<glm::vec2>& singleVerts =  singleRight ? crVerts : clVerts;
				std::vector<unsigned>&  singleIndex =  singleRight ? crIndex : clIndex;
				std::vector<glm::vec2>& otherVerts  = !singleRight ? crVerts : clVerts;
				std::vector<unsigned>&  otherIndex  = !singleRight ? crIndex : clIndex;

				AddPointToPolygon(singleVerts, singleIndex, v0);
				AddPointToPolygon(singleVerts, singleIndex, v01);
				AddPointToPolygon(singleVerts, singleIndex, v02);

				AddPointToPolygon(otherVerts, otherIndex, v1);
				AddPointToPolygon(otherVerts, otherIndex, v02);
				AddPointToPolygon(otherVerts, otherIndex, v01);

				AddPointToPolygon(otherVerts, otherIndex, v1);
				AddPointToPolygon(otherVerts, otherIndex, v2);
				AddPointToPolygon(otherVerts, otherIndex, v02);
			}
		}

		return { clVerts, clIndex, crVerts, crIndex };
	}

	void TransformPolygon(
		std::vector<glm::vec2>& polygon,
		const Transform* transform)
	{
		for (glm::vec2& vert : polygon) {
			vert = TransformPoint<Dimension::d2>(vert, transform);
		}
	}

	AABB2 GenPolygonBounds(
		std::vector<glm::vec2>& polygon)
	{
		glm::vec2 min(FLT_MAX);
		glm::vec2 max(FLT_MIN);

		for (const glm::vec2& vert : polygon)
		{
			if (vert.x > max.x) max.x = vert.x;
			if (vert.y > max.y) max.y = vert.y;
			if (vert.x < min.x) min.x = vert.x;
			if (vert.y < min.y) min.y = vert.y;
		}

		return AABB2(min, max);
	}

	void AddPointToPolygon(
		std::vector<glm::vec2>& verts,
		std::vector<unsigned>& indices,
		glm::vec2 point)
	{
		auto itr = std::find(verts.begin(), verts.end(), point);

		if (itr == verts.end())
		{
			indices.push_back(verts.size());
			verts.push_back(point);
		}

		else {
			indices.push_back(std::distance(verts.begin(), itr));
		}
	}

	
}
}
