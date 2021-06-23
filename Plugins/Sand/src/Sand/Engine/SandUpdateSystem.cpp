#include "iw/Sand/Engine/SandUpdateSystem.h"
#include "iw/Sand/SandWorker.h"

IW_PLUGIN_SAND_BEGIN

void SandWorldUpdateSystem::Update() {
	// Tiles moved into layer

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

					std::unique_lock lock(mutex);
					
					chunkCount--;
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
