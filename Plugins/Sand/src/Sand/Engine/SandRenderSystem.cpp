#include "iw/Sand/Engine/SandRenderSystem.h"
#include "iw/Sand/SandWorker.h"

IW_PLUGIN_SAND_BEGIN

int SandWorldRenderSystem::Initialize() {
	ref<Shader> shader = Asset->Load<Shader>("shaders/texture.shader");

	if (!shader) {
		return 1;
	}

	m_texture = REF<Texture>(
		Renderer->Width()  / m_world->m_scale,
		Renderer->Height() / m_world->m_scale,
		TEX_2D, RGBA
	);
	m_texture->SetFilter(NEAREST);
	m_texture->CreateColors();
	m_texture->Clear();
	
	ref<Material> material = REF<Material>(shader);
	material->SetTexture("texture", m_texture);
	material->Set("useAlpha", (int)1);
	
	m_mesh = Renderer->ScreenQuad().MakeInstance();
	m_mesh.SetMaterial(material);

	return 0;
}

void SandWorldRenderSystem::Update() {
	bool _debugShowChunkBounds = iw::Keyboard::KeyDown(iw::C);

	unsigned int* pixels = (unsigned int*)m_texture->Colors();
	m_texture->Clear();

	auto [minCX, minCY] = m_world->GetChunkLocation(m_fx,  m_fy);
	auto [maxCX, maxCY] = m_world->GetChunkLocation(m_fx2, m_fy2);

	int chunkCount = 0;

	std::mutex mutex;
	std::condition_variable cond;

	for (int cx = minCX-1; cx <= maxCX; cx++)
	for (int cy = minCY-1; cy <= maxCY; cy++) {
		iw::SandChunk* chunk = m_world->GetChunkDirect({ cx, cy });
		if (!chunk) continue;

		{ std::unique_lock lock(mutex); chunkCount++; }

		Task->queue([&, cx, cy, chunk]() {
			int startY = iw::clamp<int>(m_fy  - chunk->m_y, 0, chunk->m_height); // Could use diry rect
			int startX = iw::clamp<int>(m_fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp<int>(m_fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp<int>(m_fx2 - chunk->m_x, 0, chunk->m_width);

			iw::Cell* cells = chunk->GetField(0).cells;

			// sand texture
			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - m_fx) + (chunk->m_y + y - m_fy) * m_texture->Width();

				iw::Cell& cell = cells[x + y * chunk->m_width];

				if (_debugShowChunkBounds) {
					if (   (y == chunk->m_minY || y == chunk->m_maxY) && (x >= chunk->m_minX && x <= chunk->m_maxX)
						|| (x == chunk->m_minX || x == chunk->m_maxX) && (y >= chunk->m_minY && y <= chunk->m_maxY))
					{
						pixels[texi] = iw::Color(0, 1, 0).to32();
					}
						
					else 
					if (    x % m_world->m_chunkWidth  == 0
						||  y % m_world->m_chunkHeight == 0)
					{
						int c = m_world->m_batchGridSize;

						int batch = (c + cx % c) % c
								  + (c + cy % c) % c * c;

						iw::Color color;

						switch (batch) {
						case 0: color = iw::Color::From255(255,   0, 255); break;
						case 1: color = iw::Color::From255(255,   0,   0); break;
						case 2: color = iw::Color::From255(255, 128,   0); break;
						case 3: color = iw::Color::From255(255, 255,   0); break;
						case 4: color = iw::Color::From255(128, 255,   0); break;
						case 5: color = iw::Color::From255(0,   255,   0); break;
						case 6: color = iw::Color::From255(0,   255, 128); break;
						case 7: color = iw::Color::From255(0,   128, 255); break;
						case 8: color = iw::Color::From255(0,     0, 255); break;
						}

						pixels[texi] = color.to32();
					}
				}

				if (cell.Type != iw::CellType::EMPTY) {
					glm::vec4 accent = cell.StyleColor.rgba();

					switch (cell.Style) {
						case iw::CellStyle::RANDOM_STATIC: {
							accent *= cell.StyleOffset;
							break;
						}
						case iw::CellStyle::SHIMMER: {
							accent *= sin(cell.StyleOffset + cell.StyleOffset * 5 * iw::TotalTime());
							break;
						}
					}

					pixels[texi] = iw::Color(cell.Color + accent).to32();
				}
			}

			{ std::unique_lock lock(mutex); chunkCount--; }
			cond.notify_one();
		});
	}

	// Wait for all chunks to be copied
	{
		std::unique_lock lock(mutex);
		cond.wait(lock, [&]() { return chunkCount == 0; });
	}

	m_texture->Update(Renderer->Device);
}


IW_PLUGIN_SAND_END
