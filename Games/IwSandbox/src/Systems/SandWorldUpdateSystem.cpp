#include "Systems/SandWorldUpdateSystem.h"
#include "plugins/iw/Sand/Workers/SimpleSandWorker.h"

int SandWorldUpdateSystem::Initialize() {


	return 0;
}

void SandWorldUpdateSystem::Update() {
	// Paste tiles

	Space->Query<Tile>().Each([&](
		auto entity,
		Tile* tile)
	{
		for (auto [x, y] : tile->Positions) {
			x += ceil(tile->X);
			y += ceil(tile->Y);

			if (m_world.InBounds(x, y)) {
				// what happens if the cell is already full?
				m_world.SetCell(x, y, iw::Cell::GetDefault(iw::CellType::ROCK));
			}
		}		
	});

	// Update cells

	m_world.RemoveEmptyChunks();

	std::mutex mutex;
	std::condition_variable cond;

	auto doForAllChunks = [&](std::function<void(iw::SandChunk*)> func) {
		int chunkCount = m_world.m_chunks.size();
			
		for (iw::SandChunk* chunk : m_world.m_chunks) {
			Task->queue([&, chunk]() {
				func(chunk);

				{ std::unique_lock lock(mutex); chunkCount--; }
				cond.notify_one();
			});
		}

		std::unique_lock lock(mutex);
		cond.wait(lock, [&]() { return chunkCount == 0; });
	};
		
	doForAllChunks([&](iw::SandChunk* chunk) {
		iw::SimpleSandWorker(m_world, chunk).UpdateChunk();
	});

	doForAllChunks([&](iw::SandChunk* chunk) {
		chunk->CommitCells();
	});

	for (iw::SandChunk* chunk : m_world.m_chunks) {
		chunk->UpdateRect();
	}

	// Update the sand texture



	bool _debugShowChunkBounds =! iw::Keyboard::KeyDown(iw::C);

	unsigned int* pixels = (unsigned int*)m_texture->Colors();
	m_texture->Clear();

	auto [minCX, minCY] = m_world.GetChunkLocation(m_fx,  m_fy);
	auto [maxCX, maxCY] = m_world.GetChunkLocation(m_fx2, m_fy2);

	int chunkCount = 0;

	for (int cx = minCX-1; cx <= maxCX; cx++)
	for (int cy = minCY-1; cy <= maxCY; cy++) {
		iw::SandChunk* chunk = m_world.GetChunkDirect({ cx, cy });
		if (!chunk) continue;

		{ std::unique_lock lock(mutex); chunkCount++; }

		Task->queue([&, cx, cy, chunk]() {
			int startY = iw::clamp<int>(m_fy  - chunk->m_y, 0, chunk->m_height); // Could use diry rect
			int startX = iw::clamp<int>(m_fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp<int>(m_fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp<int>(m_fx2 - chunk->m_x, 0, chunk->m_width);

			// sand texture
			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - m_fx) + (chunk->m_y + y - m_fy) * m_texture->Width();

				const iw::Cell& cell = chunk->m_cells[x + y * chunk->m_width];
				if (cell.Type != iw::CellType::EMPTY) {
					pixels[texi] = cell.Color;
				}

				if (_debugShowChunkBounds) {
					if (   (y == chunk->m_minY || y == chunk->m_maxY) && (x >= chunk->m_minX && x <= chunk->m_maxX)
						|| (x == chunk->m_minX || x == chunk->m_maxX) && (y >= chunk->m_minY && y <= chunk->m_maxY))
					{
						pixels[texi] = iw::Color(0, 1, 0);
					}
						
					else 
					if (    x % m_world.m_chunkWidth  == 0
						||  y % m_world.m_chunkHeight == 0)
					{
						pixels[texi] = iw::Color(1, 0, 0);
					}
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
		
	// Remove tiles

	Space->Query<Tile>().Each([&](
		auto entity,
		Tile* tile)
	{
		for (auto [x, y] : tile->Positions) {
			x += ceil(tile->X);
			y += ceil(tile->Y);

			if (m_world.InBounds(x, y)) {
				// what happens if the cell is already full?
				m_world.SetCell(x, y, iw::Cell::GetDefault(iw::CellType::EMPTY));
			}
		}		
	});

	//UpdateStats(cellUpdateCount, m_world.m_chunks.size());

	m_texture->Update(Renderer->Device);
}
