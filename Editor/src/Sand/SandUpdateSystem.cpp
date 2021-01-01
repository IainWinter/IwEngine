#include "Sand/SandUpdateSystem.h"
#include "Sand/Tile.h"

SandUpdateSystem::SandUpdateSystem(
	iw::Scene* scene,
	SandWorld& world,
	iw::EntityHandle cameraEntity,
	float timeStep,
	bool debugStep)
	: SystemBase("Sand update system")
	, m_scene(scene)
	, m_world(world)
	, m_cameraEntity(cameraEntity)
	, m_timeStep(timeStep)
	, m_debugStep(debugStep)
{}

int SandUpdateSystem::Initialize() {
	iw::ref<iw::Shader> sandShader = Asset->Load<iw::Shader>("shaders/texture.shader");

	iw::ref<iw::Texture> m_sandTexture = REF<iw::Texture>(
		Renderer->Width()  / m_world.m_scale,
		Renderer->Height() / m_world.m_scale,
		iw::TEX_2D, iw::RGBA
	);
	m_sandTexture->SetFilter(iw::NEAREST);
	m_sandTexture->CreateColors();
	m_sandTexture->Clear();

	iw::ref<iw::Material> sandScreenMat = REF<iw::Material>(sandShader);
	sandScreenMat->SetTexture("texture", m_sandTexture);

	m_sandMesh = Renderer->ScreenQuad().MakeInstance();
	m_sandMesh.SetMaterial(sandScreenMat);

	return 0;
}

void SandUpdateSystem::Update() {
	iw::vector2 playerLocation = Space->FindComponent<iw::Transform>(m_cameraEntity)->Position;
		
	// camera frustrum
	int fy  = playerLocation.y - m_world.m_chunkHeight / 2;
	int fx  = playerLocation.x - m_world.m_chunkWidth  / 2;
	int fy2 = fy + m_world.m_chunkHeight;
	int fx2 = fx + m_world.m_chunkWidth;

	// Move tiles (outside of this system)

	// Paste tiles

	Space->Query<iw::Transform, Tile>().Each([&](
		iw::EntityHandle entity,
		iw::Transform* transform, 
		Tile* tile)
	{
		for (size_t i = 0; i < tile->CellCount(); i++) {
			auto [x, y] = tile->Positions[i];

			iw::vector2 v = iw::vector4(x, y, 0, 1) * transform->Transformation();
			x = v.x;
			y = v.y;

			if (m_world.InBounds(x, y)) {
				if (m_world.IsEmpty(x, y)) {
					m_world.SetCell(x, y, tile->Cells[i]);
				}

				else {
					if (true /* call tile collision callback */) {
						tile->RemoveCell(i);
						i--;
					}
				}
			}
		}

		if (tile->CellCount() < tile->InitialCellCount * 0.25) {
			Space->QueueEntity(entity, iw::func_Destroy);
		}
	});

	// Update cells
	std::condition_variable chunkCountCV;
	std::mutex chunkCountMutex;
	int chunkCount = 0;

	m_stepTimer -= iw::DeltaTime();
	if (m_timeStep == 0 || (m_timeStep <= 0.0f && (!m_debugStep || iw::Keyboard::KeyDown(iw::SPACE)))) {
		m_stepTimer = m_timeStep;

		std::vector<SandChunk*> chunksUpdated;

		for (SandChunk& chunk : m_world.m_chunks) {
			if (chunk.IsAllEmpty()) continue;

			{ std::unique_lock lock(chunkCountMutex); chunkCount++; }

			chunksUpdated.push_back(&chunk);

			Task->queue([&]() {
				for (int x = chunk.m_minX; x < chunk.m_maxX; x++)
				for (int y = chunk.m_minY; y < chunk.m_maxY; y++) {
					for (SandWorker& worker : m_world.m_workers) {
						worker.UpdateChunk();
					}
				}

				{ std::unique_lock lock(chunkCountMutex); chunkCount--; }

				chunkCountCV.notify_one();
			});
		}

		chunkCountCV.wait(std::unique_lock(chunkCountMutex), [&]() { return chunkCount == 0; });

		for (SandChunk* chunk : chunksUpdated) {
			chunk->CommitMovedCells(m_world.m_currentTick);
		}
	}

	// Copy sand colors to a texture

	iw::ref<iw::Texture> texture = m_sandMesh.Material()->GetTexture("texture");
	texture->Clear();

	unsigned int* colors = (unsigned int*)texture->Colors();

	bool showChunks = iw::Keyboard::KeyDown(iw::K);

	for (SandChunk* chunk : m_world.GetVisibleChunks(fx, fy, fx2, fy2)) {
		{ std::unique_lock lock(chunkCountMutex); chunkCount++;	}

		Task->queue([=, &chunkCount, &chunkCountCV, &chunkCountMutex]() {
			int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
			int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

				if (showChunks) {
					if (x == 0 || y == 0) {
						colors[texi] = iw::Color(1, 0, 0, 1);

						//if (   randomChunkX == chunk->m_x / chunk->m_width 
						//	&& randomChunkY == chunk->m_y / chunk->m_height)
						//{
						//	colors[texi] = iw::Color(0, 0, 1, 1);
						//}
					}

					if (x == chunk->m_minX || y == chunk->m_minY) {
						colors[texi] = iw::Color(0, 1, 0, 1);
					}

					if (x == chunk->m_maxX || y == chunk->m_maxY) {
						colors[texi] = iw::Color(0, 1, 0, 1);
					}

					const Cell& cell = chunk->GetCell(x, y);

					if (cell.Type != CellType::EMPTY) {
						colors[texi] = cell.Color;
					}
				}

				else {
					colors[texi] = chunk->GetCell(x, y).Color;
				}
			}

			{
				std::unique_lock lock(chunkCountMutex);
				chunkCount--;

				chunkCountCV.notify_one();
			}
		});
	}

	{
		std::unique_lock lock(chunkCountMutex);
		chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
	}

	// Draw the texture on the screen

	texture->Update(Renderer->Device);

	Renderer->BeginScene(m_scene);
		Renderer->DrawMesh(iw::Transform(), m_sandMesh);
	Renderer->EndScene();

		
	Space->Query<iw::Transform, Tile>().Each([&](
		iw::EntityHandle entity,
		iw::Transform* transform, 
		Tile* tile)
	{
		for (size_t i = 0; i < tile->CellCount(); i++) {
			auto [x, y] = tile->Positions[i];

			iw::vector2 v = iw::vector4(x, y, 0, 1) * transform->Transformation();
			x = v.x;
			y = v.y;

			if (m_world.InBounds(x, y)) {
				const Cell& cell = m_world.GetCell(x, y);

				if (cell.TileId == tile->TileId) {
					m_world.SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
				}

				else {
					if (true /* call tile collision callback 2 */) {
						tile->RemoveCell(i);
						i--;
					}
				}
			}
		}
	});
}
