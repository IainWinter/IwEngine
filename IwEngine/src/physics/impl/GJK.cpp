#include "iw/physics/impl/GJK.h"
#include <vector>

namespace iw {
namespace Physics {
namespace impl {

	// 2D GJK

	template<>
	bool NextSimplex(
		Simplex<d2>& points,
		_vec   <d2>& direction)
	{
		switch (points.size()) {
			case 2: return Line    (points, direction);
			case 3: return Triangle(points, direction);
		}
		
		// never should be here
		return false;
	}

	_vec<d2> tripleProduct(_vec<d2> a, _vec<d2> b, _vec<d2> c) {
        _vec<d3> A(a.x, a.y, 0);
        _vec<d3> B(b.x, b.y, 0);
        _vec<d3> C(c.x, c.y, 0);

		_vec<d3> second = cross(cross(A, B), C);

        return _vec<d2>(second.x, second.y);
    }

	template<>
	bool Line(
		Simplex<d2>& points,
		_vec   <d2>& direction)
	{
		using vec_t = _vec<d2>;

		vec_t a = points[0];
		vec_t b = points[1];

		vec_t ab = b - a;
		vec_t ao =   - a;

		direction = tripleProduct(ab, ao, ab); // see if this is enough (no ab check)

		return false;
	}

	template<>
	bool Triangle(
		Simplex<d2>& points,
		_vec   <d2>& direction)
	{
		using vec_t = _vec<d2>;

		vec_t a = points[0];
		vec_t b = points[1];
		vec_t c = points[2];

		vec_t ab = b - a;
		vec_t ac = c - a;
		vec_t ao =   - a;

		vec_t abf = tripleProduct(ac, ab, ab);
		vec_t acf = tripleProduct(ab, ac, ac);

		if (SameDirection<d2>(abf, ao)) { // should see if this is good enough (no ac check)
			return Line(points = { a, b }, direction);
		}

		if (SameDirection<d2>(acf, ao)) {
			return Line(points = { a, c }, direction);
		}

		return true;
	}

	// 3D GJK

	template<>
	bool NextSimplex(
		Simplex<d3>& points,
		_vec   <d3>& direction)
	{
		switch (points.size()) {
			case 2: return Line       (points, direction);
			case 3: return Triangle   (points, direction);
			case 4: return Tetrahedron(points, direction);
		}
		
		// never should be here
		return false;
	}

	template<>
	bool Line(
		Simplex<d3>& points,
		_vec   <d3>& direction)
	{
		using vec_t = _vec<d3>;

		vec_t a = points[0];
		vec_t b = points[1];

		vec_t ab = b - a;
		vec_t ao =   - a;

		if (SameDirection<d3>(ab, ao)) {
			direction = cross(cross(ab, ao), ab);
		}

		else {
			points = { a };
			direction = ao;
		}

		return false;
	}

	template<>
	bool Triangle(
		Simplex<d3>& points,
		_vec   <d3>& direction)
	{
		using vec_t = _vec<d3>;

		vec_t a = points[0];
		vec_t b = points[1];
		vec_t c = points[2];

		vec_t ab = b - a;
		vec_t ac = c - a;
		vec_t ao =   - a;

		vec_t abc = cross(ab, ac);

		if (SameDirection<d3>(cross(abc, ac), ao)) {
			if (SameDirection<d3>(ac, ao)) {
				points = { a, c };
				direction = cross(cross(ac, ao), ac);
			}

			else {
				return Line(points = { a, b }, direction);
			}
		}

		else {
			if (SameDirection<d3>(cross(ab, abc), ao)) {
				return Line(points = { a, b }, direction);
			}

			else {
				if (SameDirection<d3>(abc, ao)) {
					direction = abc;
				}

				else {
					points = { a, c, b };
					direction = -abc;
				}
			}
		}

		return false;
	}

	template<>
	bool Tetrahedron(
		Simplex<d3>& points,
		_vec   <d3>& direction)
	{
		using vec_t = _vec<d3>;

		vec_t a = points[0];
		vec_t b = points[1];
		vec_t c = points[2];
		vec_t d = points[3];

		vec_t ab = b - a;
		vec_t ac = c - a;
		vec_t ad = d - a;
		vec_t ao =   - a;

		vec_t abc = cross(ab, ac);
		vec_t acd = cross(ac, ad);
		vec_t adb = cross(ad, ab);

		if (SameDirection<d3>(abc, ao)) return Triangle(points = { a, b, c }, direction);
		if (SameDirection<d3>(acd, ao)) return Triangle(points = { a, c, d }, direction);
		if (SameDirection<d3>(adb, ao)) return Triangle(points = { a, d, b }, direction);

		return true;
	}

	// 2D EPA

	template<>
	ManifoldPoints EPA(
		const Simplex <d2>& simplex,
		const Collider<d2>* colliderA, const Transform* transformA,
		const Collider<d2>* colliderB, const Transform* transformB)
	{
		using vec_t = _vec<d2>;

		std::vector<vec_t> polytope(simplex.begin(), simplex.end());
	
		vec_t  minNormal;
		float  minDistance = FLT_MAX;
		size_t minIndex = 0;
	
		size_t iterations = 0;
		while (minDistance == FLT_MAX) 
		{
			if (iterations++ > GJK_EPA_MAX_ITER) {
				break;
			}

			for (size_t i = 0; i < polytope.size(); i++) {
				vec_t a = polytope[i];
				vec_t b = polytope[(i + 1) % polytope.size()];
	
				vec_t ab = b - a;
	
				vec_t normal = normalize(vec_t(ab.y, -ab.x));
				float distance = dot(normal, a);
	
				if (distance < 0) {
					normal   *= -1;
					distance *= -1;
				}
	
				if (distance < minDistance) {
					minNormal   = normal;
					minDistance = distance;
					minIndex    = i;
				}
			}
	
			vec_t support   = Support(colliderA, transformA, colliderB, transformB, minNormal);
			float sDistance = glm::dot(minNormal, support);
	
			if (fabsf(sDistance - minDistance) > 0.001f) {
				minDistance = FLT_MAX;
				polytope.insert(polytope.begin() + minIndex + 1, support);
			}
		}
	
		if (minDistance == FLT_MAX) {
			return {};
		}

		ManifoldPoints points;
		points.Normal = -glm::vec3(minNormal, 0);
		points.PenetrationDepth = minDistance;
		points.HasCollision = true;
	
		return points;
	}

	// 3D EPA

	template<>
	ManifoldPoints EPA(
		const Simplex <d3>& simplex,
		const Collider<d3>* colliderA, const Transform* transformA,
		const Collider<d3>* colliderB, const Transform* transformB)
	{
		using vec_t = _vec<d3>;

		std::vector<vec_t> polytope(simplex.begin(), simplex.end());
		std::vector<size_t>  faces = {
			0, 1, 2,
			0, 3, 1,
			0, 2, 3,
			1, 3, 2
		};
		
		auto [normals, minFace] = GetFaceNormals(polytope, faces);

		vec_t minNormal;
		float minDistance = FLT_MAX;
		
		size_t iterations = 0;
		while (minDistance == FLT_MAX) 
		{
			minNormal   = glm::vec3(normals[minFace]);
			minDistance = normals[minFace].w;

			if (iterations++ > GJK_EPA_MAX_ITER) {
				break;
			}

			vec_t support = Support(colliderA, transformA, colliderB, transformB, minNormal);
			float sDistance = glm::dot(minNormal, support);

			if (abs(sDistance - minDistance) > 0.001f) {
				minDistance = FLT_MAX;

				std::vector<std::pair<size_t, size_t>> uniqueEdges;

				for (size_t i = 0; i < normals.size(); i++) {
					if (SameDirection<d3>(vec_t(normals[i]), support)) {
						size_t f = i * 3;

						AddIfUniqueEdge(uniqueEdges, faces, f,     f + 1);
						AddIfUniqueEdge(uniqueEdges, faces, f + 1, f + 2);
						AddIfUniqueEdge(uniqueEdges, faces, f + 2, f    );

						faces[f + 2] = faces.back(); faces.pop_back();
						faces[f + 1] = faces.back(); faces.pop_back();
						faces[f    ] = faces.back(); faces.pop_back();

						normals[i] = normals.back(); normals.pop_back();

						i--;
					}
				}

				if (uniqueEdges.size() == 0) {
					break;
				}

				std::vector<size_t> newFaces;
				for (auto [edge1, edge2] : uniqueEdges) {
					newFaces.push_back(edge1);
					newFaces.push_back(edge2);
					newFaces.push_back(polytope.size());
				}

				polytope.push_back(support);

				auto [newNormals, newMinFace] = GetFaceNormals(polytope, newFaces);

				float newMinDistance = FLT_MAX;
				for (size_t i = 0; i < normals.size(); i++) {
					if (normals[i].w < newMinDistance) {
						newMinDistance = normals[i].w;
						minFace = i;
					}
				}

				if (newNormals[newMinFace].w < newMinDistance) {
					minFace = newMinFace + normals.size();
				}

				faces  .insert(faces  .end(), newFaces  .begin(), newFaces  .end());
				normals.insert(normals.end(), newNormals.begin(), newNormals.end());
			}
		}

		if (minDistance == FLT_MAX) {
			return {};
		}

		ManifoldPoints points;

		points.Normal = -minNormal;
		points.PenetrationDepth = minDistance + 0.001f;
		points.HasCollision = true;

		return points;
	}

	std::pair<std::vector<glm::vec4>, size_t> GetFaceNormals(
		const std::vector<glm::vec3>& polytope,
		const std::vector<size_t>&  faces)
	{
		std::vector<glm::vec4> normals;
		size_t minTriangle = 0;
		float  minDistance = FLT_MAX;

		for (size_t i = 0; i < faces.size(); i += 3) {
			glm::vec3 a = polytope[faces[i    ]];
			glm::vec3 b = polytope[faces[i + 1]];
			glm::vec3 c = polytope[faces[i + 2]];

			glm::vec3 normal = glm::normalize(glm::cross(b - a, c - a));
			float distance = glm::dot(normal, a);

			if (distance < 0) {
				normal   *= -1;
				distance *= -1;
			}

			normals.emplace_back(normal, distance);

			if (distance < minDistance) {
				minTriangle = i / 3;
				minDistance = distance;
			}
		}

		return { normals, minTriangle };
	}

	void AddIfUniqueEdge(
		std::vector<std::pair<size_t, size_t>>& edges,
		const std::vector<size_t>& faces,
		size_t a,
		size_t b)
	{
		auto reverse = std::find(              //      0--<--3
			edges.begin(),                     //     / \ B /   A: 2-0
			edges.end(),                       //    / A \ /    B: 0-2
			std::make_pair(faces[b], faces[a]) //   1-->--2
		);

		if (reverse != edges.end()) {
			edges.erase(reverse);
		}

		else {
			edges.emplace_back(faces[a], faces[b]);
		}
	}
}
}
}
