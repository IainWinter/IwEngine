#include "iw/graphics/MeshFactory.h"

#define ICO_X .525731112119133606f
#define ICO_Z .850650808352039932f

namespace iw {
namespace Graphics {

namespace helpers {
	inline glm::vec4* _MakeCircleArray(
		unsigned pointCount)
	{
		glm::vec4* points = new glm::vec4[pointCount];

		float calcH = 0.0f;
		float calcV = 0.0f;

		for (int i = 0; i < pointCount; i++) { // this code is duplicated in MakeCylinder, todo: consolidate
			points[i].x = sin(calcH);
			points[i].z = cos(calcH);
			points[i].y = cos(calcV);
			points[i].w = sin(calcV);

			calcH += 2 * glm::pi<float>() / (pointCount + 1);
			calcV +=     glm::pi<float>() / (pointCount + 1);
		}

		return points;
	}
}

	// Icosphere

	static const unsigned IcoVertCount  = 12;
	static const unsigned IcoIndexCount = 60;

	static const glm::vec3 IcoVerts[] = {
		glm::vec3(-ICO_X,  0,	     ICO_Z),
		glm::vec3( ICO_X,  0,	     ICO_Z),
		glm::vec3(-ICO_X,  0,     -ICO_Z),
		glm::vec3( ICO_X,  0,     -ICO_Z),
		glm::vec3(0,		 ICO_Z,  ICO_X),
		glm::vec3(0,		 ICO_Z, -ICO_X),
		glm::vec3(0,      -ICO_Z,  ICO_X),
		glm::vec3(0,	    -ICO_Z, -ICO_X),
		glm::vec3( ICO_Z,  ICO_X,  0),
		glm::vec3(-ICO_Z,  ICO_X,  0),
		glm::vec3( ICO_Z, -ICO_X,  0),
		glm::vec3(-ICO_Z, -ICO_X,  0)
	};

	static const unsigned IcoIndex[] = {
		 1,  4,  0,
		 4,  9,	 0,
		 4,  5,	 9,
		 8,  5,	 4,
		 1,  8,	 4,
		 1, 10,	 8,
		10,  3,	 8,
		 8,  3,	 5,
		 3,  2,	 5,
		 3,  7,	 2,
		 3, 10,	 7,
		10,  6,	 7,
		 6, 11,	 7,
		 6,  0, 11,
		 6,  1,	 0,
		10,  1,	 6,
		11,  0,	 9,
		 2, 11,	 9,
		 5,  2,	 9,
		11,  2,	 7
	};

	// Tetrahedron

	static const unsigned TriVertCount  = 4;
	static const unsigned TriUvCount    = 4;
	static const unsigned TriIndexCount = 12;

	static const glm::vec3 TriVerts[] = {
		glm::vec3(cos(glm::pi<float>() * 2 * 0 / 3), -1, sin(glm::pi<float>() * 2 * 0 / 3)),
		glm::vec3(cos(glm::pi<float>() * 2 * 1 / 3), -1, sin(glm::pi<float>() * 2 * 1 / 3)),
		glm::vec3(cos(glm::pi<float>() * 2 * 2 / 3), -1, sin(glm::pi<float>() * 2 * 2 / 3)),
		glm::vec3(0, 1, 0),
	};

	static const glm::vec2 TriUvs[] = {
		glm::vec2(0,    0),
		glm::vec2(1,    0),
		glm::vec2(0.5f, 1),
		glm::vec2(1,    1),
	};

	static const unsigned TriIndex[] = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 1,
		1, 3, 2
	};

	// Cube

	static const unsigned CubeVertCount  = 24;
	static const unsigned CubeIndexCount = 36;

	static glm::vec3 CubeVerts[] = {
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1, -1),
		glm::vec3(-1, -1, -1),

		glm::vec3( 1, -1, -1),
		glm::vec3( 1, -1, -1),
		glm::vec3( 1, -1, -1),
		
		glm::vec3( 1,  1, -1),
		glm::vec3( 1,  1, -1),
		glm::vec3( 1,  1, -1),
		
		glm::vec3(-1,  1, -1),
		glm::vec3(-1,  1, -1),
		glm::vec3(-1,  1, -1),
		
		glm::vec3(-1,  1,  1),
		glm::vec3(-1,  1,  1),
		glm::vec3(-1,  1,  1),
		
		glm::vec3( 1,  1,  1),
		glm::vec3( 1,  1,  1),
		glm::vec3( 1,  1,  1),
		
		glm::vec3( 1, -1,  1),
		glm::vec3( 1, -1,  1),
		glm::vec3( 1, -1,  1),
		
		glm::vec3(-1, -1,  1),
		glm::vec3(-1, -1,  1),
		glm::vec3(-1, -1,  1),
	};

	static glm::vec2 CubeUvs[] = {
		glm::vec2(0, 0),
		glm::vec2(0, 0),
		glm::vec2(0, 0),

		glm::vec2(1, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 0),
		
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		
		glm::vec2(0, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 1),
		
		glm::vec2(0, 1),
		glm::vec2(0, 1),
		glm::vec2(0, 1),
		
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		glm::vec2(1, 1),
		
		glm::vec2(1, 0),
		glm::vec2(1, 0),
		glm::vec2(1, 0),
		
		glm::vec2(0, 0),
		glm::vec2(0, 0),
		glm::vec2(0, 0),
	};

	static const unsigned CubeIndex[] = {
		0, 6, 3,    // front
		0, 9, 6,

		8, 11, 14,  // top
		8, 14, 17,

		4,  7, 16,  // right
		4, 16, 19,

		1, 22, 13,  // left
		1, 13, 10,

		15, 12, 21, // back
		15, 21, 18,

		2, 20, 23,  // bottom
		2,  5, 20
	};

	MeshData* MakeIcosphere(
		const MeshDescription& description,
		unsigned resolution)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate an IcoSphere for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		unsigned res = (unsigned)pow(4, resolution);

		unsigned indexCount = 60 * res;
		unsigned vertCount  = 12 + (30 * res);

		unsigned* indices = new unsigned [indexCount];
		glm::vec3*  verts = new glm::vec3[vertCount];

		for (size_t i = 0; i < IcoIndexCount; i++) indices[i] = IcoIndex[i];
		for (size_t i = 0; i < IcoVertCount;  i++) verts[i]   = IcoVerts[i];

		// Verts & Index

		unsigned currentIndexCount = IcoIndexCount;
		unsigned currentVertCount  = IcoVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			detail::SubDevideVerts(verts, indices, currentIndexCount, currentVertCount);
		}

		// Makes it a sphere

		for (unsigned i = 0; i < vertCount; i++) {
			verts[i] = glm::normalize(verts[i]);
		}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] indices;

		return data;
	}

	MeshData* MakeUvSphere(
		const MeshDescription& description,
		unsigned latCount,
		unsigned lonCount)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate a UvSphere for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		unsigned indexCount = 6 * (lonCount + (latCount - 2) * lonCount);
		unsigned vertCount  = (latCount + 1) * (lonCount + 1);

		if (indexCount == 0) {
			LOG_WARNING << "Cannot generate UvSphere with lat: " << latCount << " and lon: " << lonCount << " counts!";
			return nullptr;
		}

		unsigned* indices = new unsigned[indexCount];
		glm::vec3*  verts   = new glm::vec3 [vertCount];
		glm::vec2*  uvs     = nullptr;
		
		if (description.HasBuffer(bName::UV)) {
			uvs = new glm::vec2[vertCount];
		}

		// Verts

		float lonStep = 2 * glm::pi<float>() / lonCount;
		float latStep =     glm::pi<float>() / latCount;

		size_t vert = 0;
		for (unsigned lat = 0; lat <= latCount; lat++) {
			for (unsigned lon = 0; lon <= lonCount; lon++) {
				float y = sin(-glm::pi<float>() / 2 + lat * latStep);
				float x = cos(lon * lonStep) * sin(lat * latStep);
				float z = sin(lon * lonStep) * sin(lat * latStep);

				float u = (float)lon / lonCount;
				float v = (float)lat / latCount;

				verts[vert] = glm::vec3(x, y, z);
				
				if (uvs) {
					uvs[vert] = glm::vec2(u, v);
				}

				vert++;
			}
		}

		// Index

		unsigned i = 0;
		unsigned v = lonCount + 1;
		for (unsigned lon = 0; lon < lonCount; ++lon, v++) {
			indices[i++] = lon;
			indices[i++] = v;
			indices[i++] = v + 1;
		}

		for (unsigned lat = 1; lat < latCount - 1; lat++) {
			v = lat * (lonCount + 1);
			for (unsigned lon = 0; lon < lonCount; lon++, v++) {
				indices[i++] = v;
				indices[i++] = v + lonCount + 1;
				indices[i++] = v + 1;
				indices[i++] = v + 1;
				indices[i++] = v + lonCount + 1;
				indices[i++] = v + lonCount + 2;
			}
		}

		v = (latCount - 1) * (lonCount + 1);
		for (unsigned lon = 0; lon < lonCount; lon++, v++) {
			indices[i++] = v;
			indices[i++] = v + lonCount + 1;
			indices[i++] = v + 1;
		}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (uvs) {
			data->SetBufferData(bName::UV, vertCount, uvs);
		}

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] uvs;
		delete[] indices;

		return data;
	}

	MeshData* MakeCapsule(
		const MeshDescription& description,
		unsigned resolution,
		float height,
		float radius)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate a Capsule for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		// make segments an even number
		if (resolution % 2 != 0)
			resolution++;

		// extra vertex on the seam
		int points = resolution + 1;

		unsigned indexCount = (resolution * (resolution + 1) * 2 * 3);
		unsigned vertCount  = points * (points + 1);

		unsigned* indices = new unsigned[indexCount];
		glm::vec3*  verts   = new glm::vec3 [vertCount];
		glm::vec2*  uvs     = nullptr;

		if (description.HasBuffer(bName::UV)) {
			uvs = new glm::vec2[vertCount];
		}

		// calculate points around a circle

		glm::vec4* circle = helpers::_MakeCircleArray(points);
		
		// Capsule generation vars

		float yOff = (height - radius * 2.0f) * 0.5f;
		if (yOff < 0) {
			yOff = 0;
		}

		float stepX = 1.0f / (points - 1);
		int   top   = ceil (points * 0.5f);
		int   btm   = floor(points * 0.5f);

		int v = 0;

		// Top Hemisphere

		for (int y = 0; y < top; y++) {
			for (int x = 0; x < points; x++) {
				verts[v] = glm::vec3(
					circle[x].x * circle[y].w,
					circle[y].y,
					circle[x].z * circle[y].w
				) * radius;

				verts[v].y += yOff;

				if (uvs) {
					uvs[v] = glm::vec2(
						1.0f - stepX * x,
						(verts[v].y + (height * 0.5f)) / height
					);
				}

				v++;
			}
		}

		// Bottom Hemisphere

		for (int y = btm; y < points; y++) {
			for (int x = 0; x < points; x++) {
				verts[v] = glm::vec3(
					circle[x].x * circle[y].w, 
					circle[y].y, 
					circle[x].z * circle[y].w) * radius;

				verts[v].y -= yOff;

				if(uvs) {
					uvs[v] = glm::vec2(
						1.0f - stepX * x, 
						(verts[v].y + (height * 0.5f)) / height
					);
				}

				v++;
			}
		}

		// Index, connects top and bottom

		unsigned i = 0;
		for (int y = 0; y < resolution + 1; y++) {
			for (int x = 0; x < resolution; x++) {
				indices[i++] = (y + 0) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 1;

				indices[i++] = (y + 0) * points + x + 1;
				indices[i++] = (y + 0) * points + x + 0;
				indices[i++] = (y + 1) * points + x + 1;
			}
		}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (uvs) {
			data->SetBufferData(bName::UV, vertCount, uvs);
		}

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] uvs;
		delete[] indices;
		delete[] circle;

		return data;
	}

	MeshData* MakeCylinder(
		const MeshDescription& description,
		unsigned latCount,
		unsigned lonCount,
		float topRadius,
		float botRadius)
	{
		glm::vec4* circle = helpers::_MakeCircleArray(lonCount);

		assert(false);

		return nullptr;
	}

	MeshData* MakeTetrahedron(
		const MeshDescription& description,
		unsigned int resolution)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate an Tetrahedron for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		unsigned res = (unsigned)pow(4, resolution);

		unsigned indexCount = 12 * res;
		unsigned vertCount  = 4 + (6 * res);

		unsigned* indices = new unsigned[indexCount];
		glm::vec3*  verts   = new glm::vec3 [vertCount];
		glm::vec2*  uvs     = nullptr;

		if (description.HasBuffer(bName::UV)) {
			uvs = new glm::vec2[vertCount];
		}

		memcpy(indices, TriIndex, TriIndexCount * sizeof(unsigned));
		memcpy(verts,   TriVerts, TriVertCount  * sizeof(glm::vec3));

		if (uvs) {
			memcpy(uvs, TriUvs, TriUvCount * sizeof(glm::vec2));
		}

		// Verts & Index

		unsigned currentIndexCount = TriIndexCount;
		unsigned currentUvCount    = TriUvCount;
		unsigned currentVertCount  = TriVertCount;
		for (unsigned i = 0; i < resolution; i++) {
			if (uvs) {
				detail::SubDevideUvs(uvs, indices, currentIndexCount, currentUvCount);
			}

			detail::SubDevideVerts(verts, indices, currentIndexCount, currentVertCount);
		}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (uvs) {
			data->SetBufferData(bName::UV, vertCount, uvs);
		}

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] uvs;
		delete[] indices;

		return data;
	}

	MeshData* MakePlane(
		const MeshDescription& description,
		unsigned xCount,
		unsigned zCount)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate an Plane for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		unsigned indexCount = 6 * xCount * zCount;
		unsigned vertCount  = (xCount + 1) * (zCount + 1);

		unsigned* indices = new unsigned[indexCount];
		glm::vec3*  verts   = new glm::vec3 [vertCount];
		glm::vec2*  uvs     = nullptr;

		if (description.HasBuffer(bName::UV)) {
			uvs = new glm::vec2[vertCount];
		}

		float stepX = 2.0f / xCount;
		float stepZ = 2.0f / zCount;

		float stepU = 1.0f / xCount;
		float stepV = 1.0f / zCount;

		glm::vec3 offset = -(glm::vec3(1, 0, 0) + glm::vec3(0, 0, 1));

		for (unsigned x = 0; x <= xCount; x++)
		for (unsigned z = 0; z <= zCount; z++)
		{
			unsigned i = z + x * (zCount + 1);

			verts[i] = offset + glm::vec3(x * stepX, 0, z * stepZ);
				
			if (uvs) {
				uvs[i] = glm::vec2(x * stepU, (zCount - z) * stepV);
			}
		}

		unsigned i = 0, v = 0;
		while (v <= vertCount - (zCount + 3))
		{
			indices[i++] = v;
			indices[i++] = v + 1; 
			indices[i++] = v + zCount + 1;

			indices[i++] = v + 1;
			indices[i++] = v + zCount + 2; 
			indices[i++] = v + zCount + 1;

			v++;
			if (   v != 0
				&& (v + 1) % (zCount + 1) == 0)
			{
				v++;
			}
		}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (uvs) {
			data->SetBufferData(bName::UV, vertCount, uvs);
		}

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] uvs;
		delete[] indices;

		return data;
	}

	MeshData* MakeCube(
		const MeshDescription& description,
		unsigned resolution)
	{
		if (!description.HasBuffer(bName::POSITION)) {
			LOG_WARNING << "Cannot generate a Cube for a mesh description that does not contain at least a POSITION buffer!";
			return nullptr;
		}

		unsigned indexCount = 36 /*+ resolution*/;
		unsigned vertCount  = 24  /*+ resolution*/;

		unsigned* indices = new unsigned[indexCount];
		glm::vec3* verts    = new glm::vec3[vertCount];
		glm::vec2* uvs      = nullptr;

		if (description.HasBuffer(bName::UV)) {
			uvs = new glm::vec2[vertCount];
		}

		for (int i = 0; i < CubeIndexCount; i++) indices[i] = CubeIndex[i];
		for (int i = 0; i < CubeVertCount;  i++) verts  [i] = CubeVerts[i];

		if (uvs) {
			for (int i = 0; i < CubeVertCount; i++) uvs[i] = CubeUvs[i];
		}

		//// Verts
		//
		//float lonStep = Pi2 / lonCount;
		//float latStep = Pi / latCount;
		//
		//size_t vert = 0;
		//for (unsigned lat = 0; lat <= latCount; lat++) {
		//	for (unsigned lon = 0; lon <= lonCount; lon++) {
		//		float y = sin(-Pi / 2 + lat * latStep);
		//		float x = cos(lon * lonStep) * sin(lat * latStep);
		//		float z = sin(lon * lonStep) * sin(lat * latStep);
		//
		//		float u = (float)lon / lonCount;
		//		float v = (float)lat / latCount;
		//
		//		verts[vert] = glm::vec3(x, y, z);
		//
		//		if (uvs) {
		//			uvs[vert] = glm::vec2(u, v);
		//		}
		//
		//		vert++;
		//	}
		//}
		//
		//// Index
		//
		//unsigned i = 0;
		//unsigned v = lonCount + 1;
		//for (unsigned lon = 0; lon < lonCount; ++lon, v++) {
		//	indices[i++] = lon;
		//	indices[i++] = v;
		//	indices[i++] = v + 1;
		//}
		//
		//for (unsigned lat = 1; lat < latCount - 1; lat++) {
		//	v = lat * (lonCount + 1);
		//	for (unsigned lon = 0; lon < lonCount; lon++, v++) {
		//		indices[i++] = v;
		//		indices[i++] = v + lonCount + 1;
		//		indices[i++] = v + 1;
		//		indices[i++] = v + 1;
		//		indices[i++] = v + lonCount + 1;
		//		indices[i++] = v + lonCount + 2;
		//	}
		//}
		//
		//v = (latCount - 1) * (lonCount + 1);
		//for (unsigned lon = 0; lon < lonCount; lon++, v++) {
		//	indices[i++] = v;
		//	indices[i++] = v + lonCount + 1;
		//	indices[i++] = v + 1;
		//}

		MeshData* data = new MeshData(description);

		data->SetIndexData(indexCount, indices);
		data->SetBufferData(bName::POSITION, vertCount, verts);

		if (uvs) {
			data->SetBufferData(bName::UV, vertCount, uvs);
		}

		if (description.HasBuffer(bName::NORMAL)) {
			data->GenNormals();
		}

		delete[] verts;
		delete[] uvs;
		delete[] indices;

		return data;
	}

	MeshData* GenerateFromVoxels(
		const MeshDescription& description,
		glm::vec3 min,
		glm::vec3 max,
		glm::vec3 d,
		std::vector<
		std::function<float(glm::vec3, size_t, size_t, size_t, float***)>> passes)
	{
		// Generate a cube of voxels
		// Destroy voxels 
		
		int triTable[256][16] = {
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{ 8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
			{ 3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
			{ 4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{ 4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
			{ 9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
			{ 10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
			{ 5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
			{ 5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
			{ 8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
			{ 2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{ 2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
			{ 11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
			{ 5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
			{ 11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
			{ 11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
			{ 2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
			{ 6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{ 3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
			{ 6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
			{ 6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
			{ 8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
			{ 7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
			{ 3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
			{ 0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
			{ 9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
			{ 8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
			{ 5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
			{ 0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
			{ 6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
			{ 10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{ 1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
			{ 0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
			{ 3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
			{ 6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
			{ 9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
			{ 8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
			{ 3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
			{ 10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
			{ 10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{ 2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
			{ 7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
			{ 2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
			{ 1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
			{ 11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
			{ 8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
			{ 0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
			{ 7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
			{ 7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
			{ 10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
			{ 0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
			{ 7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
			{ 6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
			{ 4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
			{ 10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
			{ 8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
			{ 1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
			{ 10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
			{ 10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
			{ 9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
			{ 7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
			{ 3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
			{ 7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
			{ 3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
			{ 6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
			{ 9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
			{ 1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
			{ 4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
			{ 7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
			{ 6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
			{ 0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
			{ 6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
			{ 0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
			{ 11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
			{ 6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
			{ 5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
			{ 9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
			{ 1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
			{ 10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
			{ 0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
			{ 11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
			{ 9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
			{ 7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
			{ 2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
			{ 9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
			{ 9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
			{ 1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
			{ 0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
			{ 10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
			{ 2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
			{ 0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
			{ 0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
			{ 9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
			{ 5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
			{ 5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
			{ 8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
			{ 9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
			{ 1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
			{ 3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
			{ 4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
			{ 9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
			{ 11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
			{ 11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
			{ 2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
			{ 9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
			{ 3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
			{ 1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
			{ 4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
			{ 0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
			{ 9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
			{ 1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{ 0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
			{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
		};

		float dX = d.x;
		float dY = d.y;
		float dZ = d.z;

		//auto [dX, dY, dZ] = d;

		std::pair<int, int> edges[12] = {
			{0, 1},
			{1, 2},
			{2, 3},
			{3, 0},
			{4, 5},
			{5, 6},
			{6, 7},
			{7, 4},
			{0, 4},
			{1, 5},
			{2, 6},
			{3, 7}
		};

		size_t X = (size_t)(max.x - min.x) / dX;
		size_t Y = (size_t)(max.y - min.y) / dY;
		size_t Z = (size_t)(max.z - min.z) / dZ;

		float*** weights = detail::new3D<float>(X, Y, Z);

		for (int p = 0; p < passes.size(); p++)
		for (size_t y = 1; y < Y - 2; y++)
		for (size_t z = 1; z < Z - 2; z++)
		for (size_t x = 1; x < X - 2; x++)
		{
			auto pass = *(passes.begin() + p);
			weights[x][y][z] += pass(
				min + d * glm::vec3(x, y, z),
				x, y, z, weights);
		}

		detail::IndexLookup lookup;
		std::vector<glm::vec3>  verts;
		std::vector<unsigned> index;

		for (size_t y = 0; y < Y - 1; y++)
		for (size_t z = 0; z < Z - 1; z++)
		for (size_t x = 0; x < X - 1; x++)
		{
			float xp = min.x + x * dX;
			float yp = min.y + y * dY;
			float zp = min.z + z * dZ;

			 //     h----g
			 //   / |   /|
			 //  e-----f |
			 //  |  d--|-c
			 //  | /   |/
			 //  a-----b
			 //
			 // each vert has a bit, can you get triangle count from number of set bits?
			
			uint8_t cubeIndex = 0;
			if (weights[x  ][y  ][z  ] > 0.0f) cubeIndex |= 1 << 0;
			if (weights[x+1][y  ][z  ] > 0.0f) cubeIndex |= 1 << 1;
			if (weights[x+1][y  ][z+1] > 0.0f) cubeIndex |= 1 << 2;
			if (weights[x  ][y  ][z+1] > 0.0f) cubeIndex |= 1 << 3;
			if (weights[x  ][y+1][z  ] > 0.0f) cubeIndex |= 1 << 4;
			if (weights[x+1][y+1][z  ] > 0.0f) cubeIndex |= 1 << 5;
			if (weights[x+1][y+1][z+1] > 0.0f) cubeIndex |= 1 << 6;
			if (weights[x  ][y+1][z+1] > 0.0f) cubeIndex |= 1 << 7;

			if (cubeIndex == 0 || cubeIndex == 255) continue;

			glm::vec3 cubeVerts[] = {
				glm::vec3(xp,      yp,      zp),
				glm::vec3(xp + dX, yp,      zp),
				glm::vec3(xp + dX, yp,      zp + dZ),
				glm::vec3(xp,      yp,      zp + dZ),
				glm::vec3(xp,      yp + dY, zp),
				glm::vec3(xp + dX, yp + dY, zp),
				glm::vec3(xp + dX, yp + dY, zp + dZ),
				glm::vec3(xp,      yp + dY, zp + dZ)
			};

			int* tris = triTable[cubeIndex];

			for (int v = 0; tris[v] != -1; v += 3) {
				auto [a0, a1] = edges[tris[v + 0]];
				auto [b0, b1] = edges[tris[v + 1]];
				auto [c0, c1] = edges[tris[v + 2]];

				glm::vec3 mid0 = (cubeVerts[a0] + cubeVerts[a1]) * 0.5f;
				glm::vec3 mid1 = (cubeVerts[b0] + cubeVerts[b1]) * 0.5f;
				glm::vec3 mid2 = (cubeVerts[c0] + cubeVerts[c1]) * 0.5f;

				verts.push_back(mid0);
				verts.push_back(mid1);
				verts.push_back(mid2);

				index.push_back(index.size()); // need to find real index
				index.push_back(index.size());
				index.push_back(index.size());
			}
		}

		detail::delete3D(weights);

		MeshData* data = new MeshData(description);
		data->SetBufferData(bName::POSITION, verts.size(), verts.data());
		data->SetIndexData(index.size(), index.data());
		
		data->GenNormals();

		return data;
	}

namespace detail {
	void SubDevideVerts(
		glm::vec3* verts,
		unsigned* index,
		unsigned& currentIndexCount,
		unsigned& currentVertCount)
	{
		IndexLookup lookup;
		unsigned* next_index = new unsigned[size_t(16U) * currentIndexCount];
		unsigned nextIndexCount = 0;

		unsigned indexCount = currentIndexCount;
		for (unsigned i = 0; i < indexCount; i += 3) {
			unsigned mid0 = CreateVertexForEdge(lookup, verts, index[i],     index[i + ((i + 1) % 3)], currentVertCount);
			unsigned mid1 = CreateVertexForEdge(lookup, verts, index[i + 1], index[i + ((i + 2) % 3)], currentVertCount);
			unsigned mid2 = CreateVertexForEdge(lookup, verts, index[i + 2], index[i + ((i + 3) % 3)], currentVertCount);

			next_index[nextIndexCount++] = index[i];	 next_index[nextIndexCount++] = mid0; next_index[nextIndexCount++] = mid2;
			next_index[nextIndexCount++] = index[i + 1]; next_index[nextIndexCount++] = mid1; next_index[nextIndexCount++] = mid0;
			next_index[nextIndexCount++] = index[i + 2]; next_index[nextIndexCount++] = mid2; next_index[nextIndexCount++] = mid1;
			next_index[nextIndexCount++] = mid0;	     next_index[nextIndexCount++] = mid1; next_index[nextIndexCount++] = mid2;
		}

		memcpy(index, next_index, nextIndexCount * sizeof(unsigned));
		currentIndexCount = nextIndexCount;

		delete[] next_index;
	}


	/*
	    0, 1, 2,
		1, 0, 3,
		2, 1, 3,
		0, 2, 3
	*/

	void SubDevideUvs(
		glm::vec2* uvs,
		const unsigned* index,
		unsigned indexCount,
		unsigned& currentUvCount)
	{
		IndexLookup lookup;
		for (unsigned i = 0; i < indexCount; i += 3) {
			CreateUvsForEdge(lookup, uvs, index[i],     index[i + ((i + 1) % 3)], currentUvCount);
			CreateUvsForEdge(lookup, uvs, index[i + 1], index[i + ((i + 2) % 3)], currentUvCount);
			CreateUvsForEdge(lookup, uvs, index[i + 2], index[i + ((i + 3) % 3)], currentUvCount);
		}
	}

	unsigned CreateVertexForEdge(
		IndexLookup& lookup,
		glm::vec3* verts,
		unsigned first,
		unsigned second,
		unsigned& currentVertCount)
	{
		IndexPair key = first < second ? IndexPair(first, second) : IndexPair(second, first);

		auto inserted = lookup.insert({ key, currentVertCount });
		if (inserted.second) {
			auto& edge0 = verts[first];
			auto& edge1 = verts[second];
			auto point = (edge0 + edge1) * 0.5f;
			verts[currentVertCount++] = point;
		}

		return inserted.first->second;
	}

	unsigned CreateUvsForEdge(
		IndexLookup& lookup,
		glm::vec2* uvs,
		unsigned first,
		unsigned second,
		unsigned& currentUvCount)
	{
		IndexPair key = first < second ? IndexPair(first, second) : IndexPair(second, first);

		auto inserted = lookup.insert({ key, currentUvCount });
		if (inserted.second) {
			auto& edge0 = uvs[first];
			auto& edge1 = uvs[second];
			auto point = (edge0 + edge1) * 0.5f;
			uvs[currentUvCount++] = point;
		}

		return inserted.first->second;
	}

	unsigned CreateVertexForEdgeVector(
		IndexLookup& lookup,
		std::vector<glm::vec3>& verts,
		glm::vec3* source,
		unsigned first,
		unsigned second,
		unsigned offset)
	{
		IndexPair key = first < second 
			? IndexPair(first  + offset, second + offset)
			: IndexPair(second + offset, first  + offset);

		auto inserted = lookup.insert({ key, verts.size() });
		if (inserted.second) {
			glm::vec3& edge0 = source[first];
			glm::vec3& edge1 = source[second];
			verts.push_back((edge0 + edge1) / 2.0f);
		}

		return inserted.first->second;
	}
}
}
}
