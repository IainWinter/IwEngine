#include "iw/Sand/Engine/SandUpdateSystem.h"
#include "iw/Sand/SandWorker.h"

IW_PLUGIN_SAND_BEGIN

void SandWorldUpdateSystem::Update() {
	// Tiles

	Space->Query<iw::Transform, Tile>().Each([&](
		auto entity,
		Transform* transform,
		Tile* tile)
	{
		if (tile->LastTransform == *transform) { // Only redraw if moved
			return;
		}

		if (tile->NeedsScan) { // only rescan polygon if source has changed
			tile->NeedsScan = false;
			tile->UpdatePolygon(
				Space->FindEntity(tile).Find<MeshCollider2>(),
				m_sx, m_sy
			);

			if (!tile->IsStatic) {
				tile->Draw(transform, Renderer->ImmediateMode());
			}
		}

		tile->ForEachInWorld(&tile->LastTransform, m_sx, m_sy, [&](
			int x, int y,
			unsigned data)
		{
			if (m_world->InBounds(x, y)) {
				m_world->SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
			}
		});

		tile->NeedsDraw = true;
		tile->LastTransform = *transform;
	});

	Space->Query<iw::Transform, Tile>().Each([&](
		auto entity,
		Transform* transform,
		Tile* tile)
	{
		if (!tile->NeedsDraw) {
			return;
		}
		
		tile->NeedsDraw = false;
		tile->ForEachInWorld(transform, m_sx, m_sy, [&](
			int x, int y,
			unsigned data)
		{
			if (m_world->InBounds(x, y)) {
				m_world->SetCell(x, y, Cell::GetDefault(CellType::ROCK));
			}
		});
	});

	// Update cells

	m_world->RemoveEmptyChunks();

	std::mutex mutex;
	std::condition_variable cond;

	auto doForAllChunks = [&](std::function<void(iw::SandChunk*)> func)
	{		
		for (auto& chunks : m_world->m_batches)
		{
			int chunkCount = chunks.size();
			
			for (iw::SandChunk* chunk : chunks) {
				Task->queue([&, chunk]() {
					func(chunk);

					{ std::unique_lock lock(mutex); chunkCount--; }
					cond.notify_one();
				});
			}

			std::unique_lock lock(mutex);
			cond.wait(lock, [&]() { return chunkCount == 0; });
		}
	};

	m_world->m_frameCount += 1;

	doForAllChunks([&](iw::SandChunk* chunk) {
		for (SandWorkerBuilderBase* builder : m_world->m_workers) {
			builder->MakeWorker(*m_world, chunk)->UpdateChunk();
		}
	});

	doForAllChunks([&](iw::SandChunk* chunk) {
		chunk->CommitCells();
	});
}


IW_PLUGIN_SAND_END
