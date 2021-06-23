#include "iw/Sand/Tile.h"
#include "iw/common/algos/polygon2.h"
#include "iw/physics/AABB.h"

IW_PLUGIN_SAND_BEGIN

Tile::Tile(
	ref<AssetManager>& asset,
	const std::string& spriteName,
	bool isStatic
)
	: IsStatic(isStatic)
{
	// Source

	m_sprite = REF<Texture>(16, 64);//asset->Load<Texture>(spriteName);
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

		m_spriteMesh.Material = tileMat;

		ref<Texture> cellTex = REF<Texture>(m_sprite->Width(), m_sprite->Height());
		cellTex->SetFilter(iw::NEAREST);
		cellTex->CreateColors();

		m_target = REF<RenderTarget>();
		m_target->AddTexture(cellTex);
	}
}

void Tile::UpdatePolygon(
	float sx, float sy)
{
	unsigned* colors = (unsigned*)GetSprite()->Colors();
	glm::vec2 size   =            GetSprite()->Dimensions();

	m_polygon = iw::MakePolygonFromField(colors, size.x, size.y, 1u)[0];
	m_index   = iw::TriangulatePolygon(m_polygon);

	if (m_polygon.size() == 0) return;

	for (glm::vec2& v : m_polygon) {
		v = (v / size - glm::vec2(0.5f)) * 2.f;
	}

	m_spriteMesh.Data->SetBufferData(bName::POSITION, m_polygon.size(), m_polygon.data());
	m_spriteMesh.Data->SetIndexData(                  m_index  .size(), m_index  .data());

	for (glm::vec2& v : m_polygon) {
		v *= glm::vec2(sx, sy) / 3.f;
	}
}

void Tile::Draw(
	Transform* transform,
	Renderer* renderer)
{
	if (IsStatic) return;

	Transform noPos;
	noPos.Rotation = transform->WorldRotation();
	noPos.Scale    = transform->WorldScale();

	glm::vec2 dim = GetSprite()->Dimensions();
	iw::AABB2 aabb(glm::vec2(0.0f), iw::TransformPoint<d2>(dim, &noPos));

	dim = aabb.Max - aabb.Min;

	m_target->Resize(ceil(abs(dim.x)), ceil(abs(dim.y)));

	//GetSprite()->Update(renderer->Device);

	renderer->Renderer::BeginScene((iw::Camera*)nullptr, m_target, true);
		renderer->Renderer::DrawMesh(noPos, m_spriteMesh);
	renderer->Renderer::EndScene();

	m_target->ReadPixels(renderer->Device);
}

void Tile::ForEachInWorld(
	iw::Transform* transform,
	float sx, float sy,
	std::function<void(int, int, unsigned)> func)
{
	ref<Texture> target = GetTarget();
	int width  = target->Width();
	int height = target->Height();

	unsigned* cells = (unsigned*)target->Colors();

	for (int y = 0; y < height; y++)
	for (int x = 0; x < width;  x++)
	{
		unsigned color = cells[x + y * width];

		if (color == 0u) continue;

		int px = ceil(x - width  / 2 + transform->WorldPosition().x * sx);
		int py = ceil(y - height / 2 + transform->WorldPosition().y * sy);

		func(px, py, color);
	}
}

IW_PLUGIN_SAND_END
