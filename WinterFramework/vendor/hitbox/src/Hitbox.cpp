#include "hitbox/Hitbox.h"
#include <algorithm>
#include <iterator>

bool is_angle_convex(const ivec2& a, const ivec2& b, const ivec2& c)
{
	return  (c.y - a.y) * (b.x - a.x)
		  - (c.x - a.x) * (b.y - a.y) < 0;
}

bool is_angle_reflex(const ivec2& a, const ivec2& b, const ivec2& c)
{
	return !is_angle_convex(a, b, c);
}

bool is_point_contained(const ivec2& a, const ivec2& b, const ivec2& c, const ivec2& p) 
{
  return   !is_angle_convex(a, p, b)
  		&& !is_angle_convex(b, p, c)
		&& !is_angle_convex(c, p, a);
}

bool is_point_vertice(const ivec2& a, const ivec2& b, const ivec2& c, const ivec2& point) 
{
	return (point == a || point == b || point == c);
}

float cross_length(const vec2& a, const vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

bool is_clockwise(const std::vector<vec2>& points)
{
	int acc = 0;
	for (int i = 1; i <= points.size(); i++)
	{
		const glm::vec2& a = points.at(i % points.size());
		const glm::vec2& b = points.at(i - 1);

		acc += (b.x - a.x) * (b.y + a.y);
	}

	return acc > 0;
}

vec2 safe_normalize(const ivec2& p)
{
	float n = sqrt((float)(p.x * p.x + p.y * p.y));

	return (n == 0) ? vec2(0.f, 0.f)
					: vec2(p.x / n, p.y / n);
}

HitboxBounds make_bounds(const std::vector<vec2>& points)
{
	HitboxBounds bounds;
	std::swap(bounds.Min, bounds.Max);
	for (const auto& v : points)
	{
		bounds.Min.x = min(bounds.Min.x, v.x);
		bounds.Min.y = min(bounds.Min.y, v.y);
		bounds.Max.x = max(bounds.Max.x, v.x);
		bounds.Max.y = max(bounds.Max.y, v.y);
	}

	return bounds;
}

bool is_polygon_convex(const std::vector<vec2>& polygon)
{
	size_t polygonSize = polygon.size();
	size_t i;

	for (i = 0; i < polygonSize; i++)
	{
		size_t prev = (i + polygonSize - 1) % polygonSize;
		size_t next = (i + 1) % polygonSize;
		if (is_angle_reflex(polygon[prev], polygon[i], polygon[next]))
		{
			break;
		}
	}

	return i == polygonSize;
}

enum class StepDirection { None, N, W, S, E };

const std::array<StepDirection, 16> g_directions =
{
	StepDirection::None,
	StepDirection::N,
	StepDirection::E,
	StepDirection::E,
	StepDirection::W,
	StepDirection::N,
	StepDirection::None,
	StepDirection::E,
	StepDirection::S,
	StepDirection::None,
	StepDirection::S,
	StepDirection::S,
	StepDirection::W,
	StepDirection::N,
	StepDirection::W,
	StepDirection::None
};

std::vector<vec2> make_contour(const bool* mask_grid, int width, int height)
{
	auto is_solid = [mask_grid, width, height](int x, int y)
	{
		return x >= 0 && y >= 0 && x < width && y < height && mask_grid[x + y * width];
	};

	ivec2 start = ivec2(0, 0);

	[&]() { // this is weird
		for (start.y = 0; start.y < height - 1; start.y++)
		for (start.x = 0; start.x < width  - 1; start.x++)
		{
			if (   is_solid(start.x, start.y)
				|| is_solid(start.x, start.y - 1))
			{
				--start.y;
				return;
			}

			if (is_solid(start.x + 1, start.y))
			{
				return;
			}
		}

		assert(false && "Found no starting point");
	}();

	std::vector<vec2> contour; // out

	StepDirection nextStep = StepDirection::None;
	ivec2 point = start;

	do {
		StepDirection cuStep = nextStep;
		bool upLeft    = is_solid(point.x, point.y);
		bool upRight   = is_solid(point.x + 1, point.y);
		bool downRight = is_solid(point.x + 1, point.y + 1);
		bool downLeft  = is_solid(point.x, point.y + 1);

		const auto state = static_cast<size_t>(upLeft | upRight << 1 | downRight << 3 | downLeft << 2);

		if (is_solid(point.x, point.y))
		{
			if (contour.empty() || (contour.back().x != point.x || contour.back().y != point.y))
			{
				contour.push_back(point);
			}
		} 

		else
		{
			vec2 pr { point.x + 1, point.y };
			if (is_solid(pr.x, pr.y))
			{
				if (contour.empty() || (contour.back() != pr && contour.front() != pr))
				{
					contour.push_back(pr);
				}
			}

			vec2 pu{ point.x, point.y + 1 };
			if (is_solid(pu.x, pu.y))
			{
				if (contour.empty() || (contour.back() != pu && contour.front() != pu))
				{
					contour.push_back(pu);
				}
			}
		}

		switch (state)
		{
			case 6:  nextStep = (cuStep == StepDirection::N ? StepDirection::W : StepDirection::E); break;
			case 9:  nextStep = (cuStep == StepDirection::E ? StepDirection::N : StepDirection::S); break;
			default: nextStep = g_directions[state];
		}

		switch (nextStep)
		{
			case StepDirection::N: --point.y; break;
			case StepDirection::W: --point.x; break;
			case StepDirection::S: ++point.y; break;
			case StepDirection::E: ++point.x; break;
			default: return contour;
		}

	} while (point != start);

	return contour;
}

std::vector<vec2> make_polygon(const std::vector<vec2>& contour, const HitboxBounds& boundingBox, int accuracy)
{
	if (contour.size() <= 3)
	{
		return contour;
	}

	float kAverageDiagonal = 400.f * static_cast<float>(std::sqrt(2));

	float h = boundingBox.Max.y - boundingBox.Min.y;
	float w = boundingBox.Max.y - boundingBox.Min.y;
	float d = sqrt(h * h + w * w);
	float scaleFactor = sqrt(d) / sqrt(kAverageDiagonal);
	float minDistance = 1.5f * scaleFactor;
	float maxDistance = 40.f * scaleFactor;

	float threshold = minDistance + maxDistance - (accuracy / 100.f) * maxDistance;

	std::vector<vec2> polygon;
	std::vector<uint8_t> vertices(contour.size() - 1, 1);

	size_t i = 0;
	size_t j = vertices.size();

	std::vector<std::pair<size_t, size_t>> indexes;

	indexes.reserve(contour.size());
	indexes.emplace_back(i, j);
	
	while (!indexes.empty())
	{
		std::tie(i, j) = indexes.back();
		indexes.pop_back();
		const auto& a = contour[i];
		const auto& b = contour[j];
		float maxDistance = 0.f;
		size_t maxIndex = i;

		for (size_t k = i + 1; k < j; k++)
		{
			auto& p = contour[k];

			float distance = 0;

			vec2 v = b - a;
			vec2 w = p - a;
			float c1 = dot(w, v);
			float c2 = dot(v, v);
			float t = c1 / c2;
			vec2 shiftedPoint = a + v * t;

				 if (a  ==  b) distance = glm::distance(p, a);
			else if (c1 <=  0) distance = glm::distance(p, a);
			else if (c2 <= c1) distance = glm::distance(p, b);
			else               distance = glm::distance(p, shiftedPoint);

			if (distance >= maxDistance)
			{
				maxDistance = distance;
				maxIndex = k;
			}
		}

		if (maxDistance < threshold)
		{
			size_t k = i + 1;
			std::fill_n(std::next(vertices.begin(), static_cast<int>(k)), j - k, 0);
			continue;
		}

		indexes.emplace_back(i, maxIndex);
		indexes.emplace_back(maxIndex, j);
	}

	for (size_t i = 0; i < vertices.size(); i++)
	{
		if (vertices[i])
		{
			polygon.push_back(contour[i]);
		}
	}

	return polygon;
}

struct Vertex
{
	ivec2 p;
	Vertex* prev;
	Vertex* next;
	bool isActive{ true };
	bool isConvex;
	bool isEar;
	float angle;

	Vertex(ivec2&& p)
		: p (std::forward<ivec2>(p))
	{}
};

void updateVertex(const std::vector<Vertex>& vertices, Vertex& vertex) 
{
	const Vertex& prev = *vertex.prev;
	const Vertex& next = *vertex.next;
	vec2 vPrev = safe_normalize(prev.p - vertex.p);
	vec2 vNext = safe_normalize(next.p - vertex.p);

	vertex.angle = vPrev.x * vNext.x + vPrev.y * vNext.y;
	vertex.isConvex = is_angle_convex(prev.p, vertex.p, next.p);
	vertex.isEar = vertex.isConvex;
	if (vertex.isConvex)
	{
		for (const Vertex& v : vertices)
		{
			if (  !is_point_vertice(prev.p, vertex.p, next.p, v.p)
				&& is_point_contained(prev.p, vertex.p, next.p, v.p))
			{
				vertex.isEar = false;
				break;
			}
		}
	}
}

std::pair<bool, std::vector<vec2>> nextEar(std::vector<Vertex>& polygon, size_t vertexCount, size_t vertexIndex) {
	std::vector<vec2> triangle;
	Vertex* ear = nullptr;

	if (vertexIndex < vertexCount - 3)
	{
		for (size_t j = 0; j < vertexCount; j++)
		{
			auto& v = polygon[j];
			if (!v.isActive || !v.isEar) continue;
			if (ear == nullptr || v.angle > ear->angle) ear = &v;
		}

		if (ear == nullptr)
		{
			polygon.clear();
			return { false, {} };
		}

		triangle = std::vector<vec2> { ear->prev->p, ear->p, ear->next->p };
		ear->isActive = false;
		ear->prev->next = ear->next;
		ear->next->prev = ear->prev;

		if (vertexIndex < vertexCount - 4) {
			updateVertex(polygon, *ear->prev);
			updateVertex(polygon, *ear->next);
		}
	} 
	
	else if (vertexIndex == vertexCount - 3)
	{
		for (size_t i = 0; i < vertexCount; i++)
		{
			const Vertex& v = polygon[i];
			if (v.isActive)
			{
				triangle = { v.prev->p, v.p, v.next->p };
				break;
			}
		}
	}
	
	else
	{
		return { false, {} };
	}

	return { true, triangle };
}

std::vector<std::vector<vec2>> convert(std::vector<vec2> polygon)
{
	std::vector<Vertex> vertices;
	std::vector<std::vector<vec2>> triangles;
	std::vector<vec2> ear;
	bool success;

	std::transform( std::make_move_iterator(polygon.begin()),
					std::make_move_iterator(polygon.end()),
					std::back_inserter(vertices),
					[](vec2&& p) { return Vertex(std::forward<vec2>(p)); });

	size_t verticesSize = vertices.size();

	for (size_t i = 0; i < verticesSize; i++)
	{
		auto& vertice = vertices[i];

		vertice.prev = &vertices[(i + verticesSize - 1) % verticesSize];
		vertice.next = &vertices[(i + 1) % verticesSize];
	}

	for (auto& vertex : vertices)
	{
		updateVertex(vertices, vertex);
	}

	size_t i = 0;
	size_t vertexCount = vertices.size();
	while ((std::tie(success, ear) = nextEar(vertices, vertexCount, i)), success)
	{
		triangles.push_back(std::move(ear));
		i++;
	}

	return triangles;
}

std::vector<std::vector<vec2>> make_multiple(std::vector<vec2> polygon)
{
	if (is_polygon_convex(polygon))
	{
		return { polygon };
	}

  	std::vector<std::vector<vec2>> triangles = convert(polygon);

	for (auto polygonA = triangles.begin(); polygonA != triangles.end(); ++polygonA)
	{
		size_t _polygonASize = polygonA->size();
		size_t _polygonBSize;

		for (size_t verticeIdx = 0; verticeIdx < _polygonASize; verticeIdx++)
		{
			decltype(polygonA) polygonB;
			const auto i = static_cast<size_t>(verticeIdx); // Make a copy to cast only once.
			const vec2& pA1 = (*polygonA)[i];
			const vec2& pA2 = (*polygonA)[(i + 1) % _polygonASize];
			
			size_t j = 0;
			bool diagonalFound = false;

			for (polygonB = std::next(polygonA); polygonB != triangles.end(); ++polygonB)
			{
				_polygonBSize = polygonB->size();

				for (; j < _polygonBSize; j++)
				{
					const vec2& pB1 = (*polygonB)[j];
					const vec2& pB2 = (*polygonB)[(j + 1) % _polygonBSize];

					if (pA2 == pB1 && pA1 == pB2)
					{
						diagonalFound = true;
						break;
					}
				}

				if (diagonalFound)
				{
					break;
				}
			}

			if (!diagonalFound)
			{
				continue;
			}
			
			size_t i2 = (i +                 1) % _polygonASize; // End of the diagonal for polygonA
			size_t i0 = (i + _polygonASize - 1) % _polygonASize; // Point preceding the diagonal for polygonA
			size_t j2 = (j +                 1) % _polygonBSize; // End of the diagonal for polygonB
			size_t j3 = (j2 +                1) % _polygonBSize; // Point after the diagonal for polygonB

			// Check the left side of the diagonal
			if (is_angle_reflex((*polygonA)[i0], (*polygonA)[i], (*polygonB)[j3])) {
				continue;
			}

			size_t i3 = (i2 +                 1) % _polygonASize; // Point after the diagonal for polygonA
			size_t j0 = (j  + _polygonBSize - 1) % _polygonBSize; // Point preceding the diagonal for polygonB

			// Check the right side of the diagonal
			if (is_angle_reflex((*polygonB)[j0], (*polygonA)[i2], (*polygonA)[i3])) {
				continue;
			}

			std::vector<vec2> polygon;
			polygon.reserve(_polygonASize + _polygonBSize - 2);
			for (size_t k = i2; k != i; k = (k + 1) % _polygonASize) polygon.push_back((*polygonA)[k]);
			for (size_t k = j2; k != j; k = (k + 1) % _polygonBSize) polygon.push_back((*polygonB)[k]);

			*polygonA = std::move(polygon);
			triangles.erase(polygonB);
			_polygonASize = polygonA->size();
			verticeIdx = 0;
		}
	}

	return triangles;
}

std::pair<std::vector<std::vector<vec2>>, HitboxBounds> make_hitbox(const bool* mask_grid, int width, int height, int accuracy)
{
	auto contour  = make_contour(mask_grid, width, height);
	auto bounds   = make_bounds(contour);
	auto polygons = make_multiple(make_polygon(contour, bounds, accuracy));
	
	float w = bounds.Width()  / 2;
	float h = bounds.Height() / 2;

	for (std::vector<vec2>& polygon : polygons)
	{
		if (is_clockwise(polygon))
		{
			std::reverse(polygon.begin(), polygon.end());
		}

		for (glm::vec2& vert : polygon)
		{
			vert.x = vert.x / w - 1;  // scale from pixel to hitbox scale and center at 0, not hitbox center
			vert.y = vert.y / h - 1;
		}
	}

	return std::make_pair(polygons, bounds);
}