#pragma once

#include "Cell.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/RenderTarget.h"
#include "iw/graphics/Renderer.h"
#include "iw/physics/AABB.h"
#include "iw/common/algos/MarchingCubes.h"
#include <vector>

struct Tile {
	// new process
private:
	iw::AABB2 m_bounds;

	bool m_aabbNeedsUpdate = true;
	bool m_sourceNeedsUpdate = true;
	bool m_initialized = false;

	glm::vec2 m_currentOffset;

	std::vector<glm::vec2> m_polygon;
	std::vector<unsigned>    m_index;

public:
	iw::Mesh Body;
	iw::ref<iw::RenderTarget> Target;

	void Initialize(
		iw::Transform* transform,
		SandWorld& world,
		iw::ref<iw::AssetManager>& asset)
	{
		if (m_initialized) return;
		m_initialized = true;

		iw::AABB2 bounds = AABB();

		int sizeX = bounds.Max.x - bounds.Min.x + 3; // not +2 because texture size is exclusive?? that shouldnt be the case tho
		int sizeY = bounds.Max.y - bounds.Min.y + 3;

		iw::ref<iw::Texture> texture = REF<iw::Texture>(sizeX, sizeY);
		texture->SetFilter(iw::NEAREST);
		texture->SetMipmapFilter(iw::NEAREST_NEAREST);

		unsigned* colors = (unsigned*)texture->CreateColors();

		for (glm::vec2& v : Locations) {
			int x = v.x + 1; // I think this is always safe
			int y = v.y + 1;
			int i = x + y * sizeX;
			colors[i] = ((unsigned)Cell::GetDefault(CellType::METAL).Type << 24) | i;
		}

		m_polygon = iw::common::MakePolygonFromField(colors, sizeX, sizeY, 1u);
		m_index   = iw::common::TriangulatePolygon(m_polygon);

		for (glm::vec2& v : m_polygon) v = v / glm::vec2(sizeX, sizeY);

		std::vector<glm::vec2> uv = m_polygon;

		for (glm::vec2& v : m_polygon) {
			v = (v - 0.5) * 2;
			v *= glm::vec2(sizeX - 3, sizeY - 3) / glm::vec2(sizeX, sizeY);
		}

		//// this may not be needed
		//glm::vec2 avg;
		//for (glm::vec2& v : m_polygon) { avg += v; } avg /= m_polygon.size();
		//for (glm::vec2& v : m_polygon) { v -= avg; }

		iw::ref<iw::Material> material = REF<iw::Material>(asset->Load<iw::Shader>("shaders/texture2D.shader"));
		material->SetTexture("texture", texture);

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

		Body = (new iw::MeshData(description))->MakeInstance();
		Body.Data()->SetBufferData(iw::bName::UV, uv.size(), uv.data());
		Body.SetMaterial(material);
	}

	void Update(float angle, iw::Renderer* renderer) {
		if (!m_initialized) return; // Tried to update before initialize, happens on enemy spawn

		iw::AABB2 bounds = AABB();

		glm::vec2 center = bounds.Center();
		glm::vec2 minXmaxY(bounds.Min.x, bounds.Max.y);
		glm::vec2 maxXminY(bounds.Max.x, bounds.Min.y);
			
		glm::vec2 a = TranslatePoint(bounds.Min - center, center, angle);
		glm::vec2 b = TranslatePoint(bounds.Max - center, center, angle);
		glm::vec2 c = TranslatePoint(minXmaxY   - center, center, angle);
		glm::vec2 d = TranslatePoint(maxXminY   - center, center, angle);

		glm::vec2 min = FLT_MAX;
		glm::vec2 max = FLT_MIN;
		for (glm::vec2 v : {a, b, c, d}) {
			if (v.x < min.x) min.x = v.x;
			if (v.y < min.y) min.y = v.y;
			if (v.x > max.x) max.x = v.x;
			if (v.y > max.y) max.y = v.y;
		}

		int sizeX = max.x - min.x;
		int sizeY = max.y - min.y;

		iw::ref<iw::Texture> source = Body.Material()->GetTexture("texture");
		glm::vec2 srcDim = source->Dimensions();
		glm::vec2 newDim = glm::vec2(sizeX, sizeY);

		if (m_sourceNeedsUpdate) {
			m_sourceNeedsUpdate = false;
			source->Update(renderer->Device);
		}

		bool scalingNeedsUpdate = !Target || sizeX != Target->Width() || sizeY != Target->Height();

		if (scalingNeedsUpdate) {
			LOG_INFO << "Rescaled";

			std::vector<glm::vec2> polygon = m_polygon;
			for (glm::vec2& v : polygon) {
				v *= srcDim / newDim; // todo: this eqwuation is wrong, correct one needs to be found
			}

			Body.Data()->SetBufferData(iw::bName::POSITION, polygon.size(), polygon.data());
			Body.Data()->SetIndexData (                     m_index.size(), m_index.data());
		}

		if (!Target) {
			iw::ref<iw::Texture> out = REF<iw::Texture>(sizeX, sizeY);
			out->SetFilter(iw::NEAREST);
			out->SetMipmapFilter(iw::NEAREST_NEAREST);
			out->CreateColors();

			Target = REF<iw::RenderTarget>();
			Target->AddTexture(out);
		}

		else if (scalingNeedsUpdate) {
			Target->Resize(sizeX, sizeY);
		}

		iw::quaternion rot = iw::quaternion::from_euler_angles(iw::Pi/*messy*/, 0, angle);

		//renderer->iw::Renderer::Begin(); // queue these?, todo: queued renderer shouldnt derive renderer, just take one
		renderer->iw::Renderer::BeginScene((iw::Camera*)nullptr, Target, true);
		renderer->iw::Renderer::DrawMesh(iw::Transform(0, 1, rot), Body);
		renderer->iw::Renderer::EndScene();
		//renderer->iw::Renderer::End();

		Target->ReadPixels(renderer->Device);

		m_currentOffset = (srcDim - newDim) / 2;
	}

	void RemoveCell(
		size_t index,
		iw::Renderer* renderer)
	{
		auto [field, sizeX, sizeY] = GetSourceField();

		size_t x = index % sizeX;
		size_t y = index / sizeX;

		field[x + y * sizeX] = 0;
		m_aabbNeedsUpdate = true; // could be more efficient
		m_sourceNeedsUpdate = true;
	}

	//void EjectTriangle(
	//	size_t triIndex,
	//	iw::ref<iw::Space>& space)
	//{
	//	size_t index = triIndex / 3;

	//	unsigned tri[3]{
	//		m_index.at(index + 2),
	//		m_index.at(index + 1),
	//		m_index.at(index + 0),
	//	};

	//	m_index.erase(m_index.begin() + index + 2);
	//	m_index.erase(m_index.begin() + index + 1);
	//	m_index.erase(m_index.begin() + index + 0);

	//	iw::Mesh body = Body.MakeCopy(Body.Data()->Description(), false);
	//	body.Data()->SetIndexData(3, tri);

	//	Tile tile();
	//}

	glm::vec2 GetCurrentOffset() {
		return m_currentOffset;
	}

	std::tuple<unsigned*, size_t, size_t> GetSourceField() {
		if (!m_initialized) return { nullptr, 0, 0 };

		iw::ref<iw::Texture> field = Body.Material()->GetTexture("texture");
		return { (unsigned*)field->Colors(), field->Width(), field->Height() };
	}

	std::tuple<unsigned*, size_t, size_t> GetCurrentField() {
		if (!Target) return { nullptr, 0, 0};

		iw::ref<iw::Texture> field = Target->Tex(0);
		return { (unsigned*)field->Colors(), field->Width(), field->Height() };
	}

// old process

	//std::vector<glm::vec2> FullLocations; // copy for healing / frame
	//std::vector<glm::vec2> Locations;
	//const int InitialLocationsSize = 0;
	//int TileId = 0;

	//int Precedence = 10;

	//bool ExplodeOnDeath = true;

	//Tile() = default;

	//Tile(std::vector<glm::vec2> locations, int scale)
	//	: InitialLocationsSize(locations.size() * scale)
	//{
	//	for (glm::vec2& v : locations) {
	//		for (int y = 0; y <= abs(scale); y++) 
	//		for (int x = 0; x <= abs(scale); x++) {
	//			Locations.push_back(v * scale + glm::vec2(x, y)/* - scale/2*/);
	//		}
	//	}

	//	static int s_tileId = 1;
	//	TileId = s_tileId++;

	//	FullLocations = Locations;
	//}

	//glm::vec2 FurthestPoint(glm::vec2 d) {
	//	glm::vec2 furthest;
	//	float maxDist = FLT_MIN;
	//	for (glm::vec2& v : Locations) {
	//		float dist = v.dot(d);
	//		if (dist > maxDist) {
	//			maxDist = dist;
	//			furthest = v;
	//		}
	//	}

	//	return furthest;
	//}

	float Radius() {
		glm::vec2 furthest;
		for (glm::vec2& v : Locations) {
			if (v.length_squared() > furthest.length_squared()) {
				furthest = v;
			}
		}

		return furthest.length();
	}

	iw::AABB2 AABB() {
		if (!m_aabbNeedsUpdate) return m_bounds;
		m_aabbNeedsUpdate = false;

		iw::AABB2 bounds;
		for (glm::vec2& v : Locations) {
			if (bounds.Min.x > v.x) bounds.Min.x = v.x;
			if (bounds.Min.y > v.y) bounds.Min.y = v.y;

			if (bounds.Max.x < v.x) bounds.Max.x = v.x;
			if (bounds.Max.y < v.y) bounds.Max.y = v.y;
		}

		return m_bounds = bounds;
	}
};

//struct Tile {
//	std::vector<WorldCoords> Positions;
//	std::vector<Cell> Cells;
//
//	const int InitialCellCount = 0;
//	int TileId = 0;
//
//	Tile() = default;
//
//	Tile(
//		std::vector<WorldCoords>&& positions,
//		std::vector<Cell>&& cells)
//		: Positions(positions)
//		, Cells(cells)
//		, InitialCellCount(CellCount())
//	{
//		assert(positions.size() == cells.size()); // dont assert, fill cells or remove cells based on positions
//
//		static int tileId = 0;
//		TileId = ++tileId;
//
//		for (Cell& c : Cells) {
//			c.TileId = TileId;
//		}
//	}
//
//	void RemoveCell(size_t i) {
//		Positions[i] = Positions.back(); Positions.pop_back();
//		Cells    [i] = Cells    .back(); Cells    .pop_back();
//	}
//
//	size_t CellCount() {
//		return Positions.size();
//	}
//};
