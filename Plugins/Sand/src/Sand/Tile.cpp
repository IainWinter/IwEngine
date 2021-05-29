#include "iw/Sand/Tile.h"
#include "iw/common/algos/MarchingCubes.h"

IW_PLUGIN_SAND_BEGIN

Tile::Tile(
	ref<AssetManager>& asset,
	const std::string& spriteName,
	bool isStatic
)
	: IsStatic(isStatic)
{
	// Source

	m_sprite = REF<Texture>(64, 64);//asset->Load<Texture>(spriteName);
	m_sprite->SetFilter(iw::NEAREST);
	m_sprite->CreateColors();

	MeshDescription tileDesc;
	tileDesc.DescribeBuffer(bName::POSITION, MakeLayout<float>(2));

	m_spriteMesh = (new MeshData(tileDesc))->MakeInstance();

	// Target

	if (!IsStatic)
	{
		ref<Material> tileMat = REF<Material>(asset->Load<Shader>("shaders/texture2d.shader"));
		tileMat->SetTexture("texture", m_sprite);

		m_spriteMesh.SetMaterial(tileMat);

		ref<Texture> cellTex = REF<Texture>(m_sprite->Width(), m_sprite->Height());
		cellTex->SetFilter(iw::NEAREST);
		cellTex->CreateColors();

		m_target = REF<RenderTarget>();
		m_target->AddTexture(cellTex);
	}
}

void Tile::UpdatePolygon(
	MeshCollider* collider)
{
	unsigned* colors = (unsigned*)GetSprite()->Colors();
	glm::vec2 size   =            GetSprite()->Dimensions();

	std::vector<glm::vec2> m_polygon = iw::common::MakePolygonFromField(colors, size.x, size.y, 0u);
	std::vector<unsigned>  m_index   = iw::common::TriangulatePolygon(m_polygon);

	if (m_polygon.size() == 0) return;

	for (glm::vec2& v : m_polygon)
	{
		v = (v / size - glm::vec2(0.5f)) * glm::vec2(2.0f);
	}

	m_spriteMesh.Data()->SetBufferData(bName::POSITION, m_polygon.size(), m_polygon.data());
	m_spriteMesh.Data()->SetIndexData(                  m_index  .size(), m_index  .data());

	auto [verts, vcount] = GetPolygon();
	auto [index, icount] = GetIndex();

	for (glm::vec2* v = verts; v != verts + vcount; v++)
	{
		collider->AddPoint(glm::vec3(v->x, v->y, 0));
	}

	for (unsigned* i = index; i != index + icount; i += 3)
	{
		collider->AddTriangle(*(i), *(i + 1), *(i + 2));
	}
}

void Tile::Draw(
	Transform* transform,
	Renderer* renderer)
{
	if (!IsStatic) return;

	Transform onlyRot;
	onlyRot.Rotation = transform->WorldRotation();

	renderer->Renderer::BeginScene((iw::Camera*)nullptr, m_target, true);
		renderer->Renderer::DrawMesh(onlyRot, m_spriteMesh);
	renderer->Renderer::EndScene();

	m_target->ReadPixels(renderer->Device);
}

void Tile::ForEachInWorld(
	iw::Transform* transform,
	std::function<void(int, int, unsigned)> func)
{
	ref<Texture> target = GetTarget();
	unsigned width  = target->Width();
	unsigned height = target->Height();

	unsigned* cells = (unsigned*)target->Colors();

	for (unsigned y = 0; y < height; y++)
	for (unsigned x = 0; x < width;  x++)
	{
		unsigned color = cells[x + y * width];

		if (color == 0) continue;

		int px = ceil(transform->WorldPosition().x + x);
		int py = ceil(transform->WorldPosition().y + y);

		func(px, py, color);
	}
}

IW_PLUGIN_SAND_END
