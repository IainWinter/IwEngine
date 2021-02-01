#pragma once

#include "Cell.h"
#include <vector>
#include "iw/graphics/Mesh.h"
#include "iw/graphics/RenderTarget.h"
#include "iw/graphics/Renderer.h"
#include "iw/physics/AABB.h"
#include "iw/common/algos/MarchingCubes.h"

struct Tile {
	// new process
private:
	iw::AABB2 m_bounds;

	bool m_needsUpdate = true;
	bool m_initialized = false;

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

		int sizeX = bounds.Max.x - bounds.Min.x+3;
		int sizeY = bounds.Max.y - bounds.Min.y+3;

		iw::ref<iw::Texture> texture = REF<iw::Texture>(sizeX, sizeY);
		texture->SetFilter(iw::NEAREST);
	
		unsigned* colors = (unsigned*)texture->CreateColors();

		for (iw::vector2& v : Locations) {
			int x = v.x + 1; // I think this is always safe
			int y = v.y + 1;
			colors[x + y * sizeX] = Cell::GetDefault(CellType::METAL).Color;
		}

		std::vector<iw::vector2> polygon = iw::common::MakePolygonFromField(colors, sizeX, sizeY, 1u);
		std::vector<unsigned>    index   = iw::common::TriangulatePolygon(polygon);

		iw::vector2 avgv;
		iw::vector2 minv = FLT_MAX;
		for (iw::vector2& v : polygon) {         // somewhere the scaling is off
			v /= iw::vector2(sizeX, sizeY);
			avgv += v;
			if (minv.length_squared() > v.length_squared()) minv = v;
		}

		std::vector<iw::vector2> uvs = polygon;

		for (iw::vector2& v : uvs) {
			v += minv;
		}

		avgv /= polygon.size();
		for (iw::vector2& v : polygon) {
			v -= avgv;
			///v *= 2;
		}

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

		iw::MeshData* data = new iw::MeshData(description);
		data->SetBufferData(iw::bName::POSITION, polygon.size(), polygon.data());
		data->SetBufferData(iw::bName::UV,       uvs    .size(), uvs    .data());
		data->SetIndexData(index.size(), index.data());

		iw::ref<iw::Material> material = REF<iw::Material>(asset->Load<iw::Shader>("shaders/texture2D.shader"));
		material->SetTexture("texture", texture);
		//material->SetWireframe(true);

		Body = data->MakeInstance();
		Body.SetMaterial(material);
	}

	void Update(float angle, iw::Renderer* renderer) {
		if (!m_needsUpdate) return;

		iw::AABB2 bounds = AABB();

		iw::vector2 center = bounds.Center();
		iw::vector2 minXmaxY(bounds.Min.x, bounds.Max.y);
		iw::vector2 maxXminY(bounds.Max.x, bounds.Min.y);
			
		iw::vector2 a = TranslatePoint(bounds.Min - center, center, angle);
		iw::vector2 b = TranslatePoint(bounds.Max - center, center, angle);
		iw::vector2 c = TranslatePoint(minXmaxY   - center, center, angle);
		iw::vector2 d = TranslatePoint(maxXminY   - center, center, angle);

		iw::vector2 min = FLT_MAX;
		iw::vector2 max = FLT_MIN;
		for (iw::vector2 v : {a, b, c, d}) {
			if (v.x < min.x) min.x = v.x;
			if (v.y < min.y) min.y = v.y;
			if (v.x > max.x) max.x = v.x;
			if (v.y > max.y) max.y = v.y;
		}

		int sizeX = max.x - min.x;
		int sizeY = max.y - min.y;

		iw::ref<iw::Texture> out = REF<iw::Texture>(sizeX, sizeY);
		out->CreateColors();

		Target = REF<iw::RenderTarget>();
		Target->AddTexture(out);

		iw::quaternion rot = iw::quaternion::from_euler_angles(iw::Pi/*messy*/, 0, angle);

		renderer->Begin();
		renderer->BeginScene((iw::Camera*)nullptr, Target, true);
		renderer->DrawMesh(iw::Transform(0, 1, rot), Body);
		renderer->EndScene();
		renderer->End();

		Target->ReadPixels(renderer->Device);
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

	std::vector<iw::vector2> FullLocations; // copy for healing / frame
	std::vector<iw::vector2> Locations;
	const int InitialLocationsSize = 0;
	int TileId = 0;

	int Precedence = 10;

	bool ExplodeOnDeath = true;

	Tile() = default;

	Tile(std::vector<iw::vector2> locations, int scale)
		: InitialLocationsSize(locations.size() * scale)
	{
		for (iw::vector2& v : locations) {
			for (int x = 0; x < abs(scale); x++)
			for (int y = 0; y < abs(scale); y++) {
				Locations.push_back(v * scale + iw::vector2(x, y)/* - scale/2*/);
			}
		}

		static int s_tileId = 1;
		TileId = s_tileId++;

		FullLocations = Locations;
	}

	iw::vector2 FurthestPoint(iw::vector2 d) {
		iw::vector2 furthest;
		float maxDist = FLT_MIN;
		for (iw::vector2& v : Locations) {
			float dist = v.dot(d);
			if (dist > maxDist) {
				maxDist = dist;
				furthest = v;
			}
		}

		return furthest;
	}

	float Radius() {
		iw::vector2 furthest;
		for (iw::vector2& v : Locations) {
			if (v.length_squared() > furthest.length_squared()) {
				furthest = v;
			}
		}

		return furthest.length();
	}

	iw::AABB2 AABB() {
		if (!m_needsUpdate) return m_bounds;

		iw::AABB2 bounds;
		for (iw::vector2& v : Locations) {
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
