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
	
	m_mesh = ScreenQuad().MakeInstance();
	m_mesh.Material = (material);

	return 0;
}

void SandWorldRenderSystem::Update() {
	bool _debugShowChunkBounds = Keyboard::KeyDown(C);

	unsigned int* pixels = (unsigned int*)m_texture->Colors();
	m_texture->Clear();

	auto [minCX, minCY] = m_world->GetChunkLocation(m_fx,  m_fy);
	auto [maxCX, maxCY] = m_world->GetChunkLocation(m_fx2, m_fy2);

	int chunkCount = 0;

	std::mutex mutex;
	std::condition_variable cond;

	for (int cx = minCX-1; cx <= maxCX; cx++)
	for (int cy = minCY-1; cy <= maxCY; cy++) {
		SandChunk* chunk = m_world->GetChunkDirect({ cx, cy });
		if (!chunk) continue;

		{ std::unique_lock lock(mutex); chunkCount++; }

		Task->queue([&, cx, cy, chunk]() {
			int startY = clamp<int>(m_fy  - chunk->m_y, 0, chunk->m_height); // Could use diry rect
			int startX = clamp<int>(m_fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = clamp<int>(m_fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = clamp<int>(m_fx2 - chunk->m_x, 0, chunk->m_width);

			Cell*     cells  = chunk->GetField(SandField::CELL) .GetCells<Cell>();
			uint32_t* colors = chunk->GetField(SandField::COLOR).GetCells<uint32_t>();

			// sand texture
			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - m_fx) + (chunk->m_y + y - m_fy) * m_texture->Width();

				// annoying but nessesary for style rn
				Cell & cell  = cells [x + y * chunk->m_width]; 
				Color& color = Color::From32(colors[x + y * chunk->m_width]);

				if (_debugShowChunkBounds) {
					if (   (y == chunk->m_minY || y == chunk->m_maxY) && (x >= chunk->m_minX && x <= chunk->m_maxX)
						|| (x == chunk->m_minX || x == chunk->m_maxX) && (y >= chunk->m_minY && y <= chunk->m_maxY))
					{
						pixels[texi] = Color(0, 1, 0).to32();
					}
						
					else 
					if (    x % m_world->m_chunkWidth  == 0
						||  y % m_world->m_chunkHeight == 0)
					{
						int c = m_world->m_batchGridSize;

						int batch = (c + cx % c) % c
								  + (c + cy % c) % c * c;

						Color color;

						switch (batch) {
						case 0: color = Color::From255(255,   0, 255); break;
						case 1: color = Color::From255(255,   0,   0); break;
						case 2: color = Color::From255(255, 128,   0); break;
						case 3: color = Color::From255(255, 255,   0); break;
						case 4: color = Color::From255(128, 255,   0); break;
						case 5: color = Color::From255(0,   255,   0); break;
						case 6: color = Color::From255(0,   255, 128); break;
						case 7: color = Color::From255(0,   128, 255); break;
						case 8: color = Color::From255(0,     0, 255); break;
						}

						pixels[texi] = color.to32();
					}
				}

				if (cell.Type != CellType::EMPTY) {
					glm::vec4 accent = cell.StyleColor.rgba();

					switch (cell.Style) {
						case CellStyle::RANDOM_STATIC: {
							accent *= cell.StyleOffset;
							break;
						}
						case CellStyle::SHIMMER: {
							accent *= sin(cell.StyleOffset + cell.StyleOffset * 5 * TotalTime());
							break;
						}
					}

					pixels[texi] = Color(color + accent).to32();
				}
			}

			std::unique_lock lock(mutex); 
			
			chunkCount--;
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
