#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"

int __chunkScale = 1;
int __brushSizeX = 50;
int __brushSizeY = 50;
float __stepTimeThresh = 0;

namespace iw {
	SandLayer::SandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(100, 100, __chunkScale))
		, reset(true)
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		if (int err = Layer::Initialize()) {
			return err;
		}

		// Font

		m_font = Asset->Load<iw::Font>("fonts/arial.fnt");
		m_font->Initialize(Renderer->Device);

		iw::ref<iw::Shader> fontShader = Asset->Load<Shader>("shaders/font_simple.shader");
		Renderer->InitShader(fontShader, CAMERA);

		ref<Material> textMat = REF<Material>(fontShader);
		textMat->Set("color", iw::Color(1));
		textMat->SetTexture("fontMap", m_font->GetTexture(0));

		Asset->Give<Material>("materials/Font", textMat);

		m_textMesh = m_font->GenerateMesh("", .005f, 1);
		m_textMesh.SetMaterial(textMat);

		// Sand

		Cell empty  = { CellType::EMPTY };
		Cell sand   = { CellType::SAND,  CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };
		Cell water  = { CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE };
		Cell rock   = { CellType::ROCK  };
		Cell metal  = { CellType::METAL };
		Cell debris = { CellType::DEBRIS };
		Cell laser  = { CellType::LASER,  CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell elaser = { CellType::eLASER, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell bullet = { CellType::BULLET, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_PROJECTILE };

		empty  .Color = iw::Color::From255(  0,   0,   0, 0);
		sand   .Color = iw::Color::From255(237, 201, 175);
		water  .Color = iw::Color::From255(175, 201, 237);
		rock   .Color = iw::Color::From255(201, 201, 201);
		metal  .Color = iw::Color::From255(230, 230, 230);
		debris .Color = iw::Color::From255(150, 150, 150);
		laser  .Color = iw::Color::From255(255,   0,   0);
		elaser .Color = iw::Color::From255(  0, 200, 255);
		bullet .Color = iw::Color::From255(255, 255,   0);

		sand .dY = -1;
		water.dY = -1;

		laser .Life = 0.06f;
		elaser.Life = 1.00f;
		bullet.Life = 0.02f;

		Cell::SetDefault(CellType::EMPTY,  empty);
		Cell::SetDefault(CellType::SAND,   sand);
		Cell::SetDefault(CellType::WATER,  water);
		Cell::SetDefault(CellType::ROCK,   rock);
		Cell::SetDefault(CellType::METAL,  metal);
		Cell::SetDefault(CellType::DEBRIS, debris);
		Cell::SetDefault(CellType::LASER,  laser);
		Cell::SetDefault(CellType::eLASER, elaser);
		Cell::SetDefault(CellType::BULLET, bullet);

		// Sand rendering

		iw::ref<iw::Shader> sandShader = Asset->Load<Shader>("shaders/texture.shader");

		m_sandTexture = REF<iw::Texture>(
			Renderer->Width()  / world.m_scale,
			Renderer->Height() / world.m_scale, 
			iw::TEX_2D,
			iw::RGBA
		);
		m_sandTexture->SetFilter(iw::NEAREST);
		m_sandTexture->CreateColors();
		m_sandTexture->Clear();

		iw::ref<iw::Material> sandScreenMat = REF<iw::Material>(sandShader);
		sandScreenMat->SetTexture("texture", m_sandTexture);

		m_sandScreen = Renderer->ScreenQuad().MakeInstance();
		m_sandScreen.SetMaterial(sandScreenMat);

		// Stars

		iw::ref<iw::Shader> starsShader = Asset->Load<Shader>("shaders/particle/simple_point.shader");

		iw::ref<iw::Material> starMat = REF<iw::Material>(starsShader);
		starMat->Set("color", iw::Color(1));

		iw::Mesh starMesh = Renderer->ScreenQuad().MakeCopy();

		iw::vector3* p  = (iw::vector3*)starMesh.Data()->Get(bName::POSITION);
		iw::vector2* u  = (iw::vector2*)starMesh.Data()->Get(bName::UV);
		unsigned int* i = starMesh.Data()->GetIndex();

		starMesh.Data()->SetTopology(iw::MeshTopology::POINTS);
		starMesh.Data()->SetBufferData(bName::POSITION, 1, p);
		starMesh.Data()->SetBufferData(bName::UV,       1, u);
		starMesh.Data()->SetIndexData(1, i);
		starMesh.SetMaterial(starMat);

		m_stars.SetParticleMesh(starMesh);
		m_stars.SetTransform(new iw::Transform(0, 0.1f));

		for (int i = 0; i < 2000; i++) {
			m_stars.SpawnParticle(iw::Transform(
				iw::vector2(Renderer->Width() * iw::randf() / 2,
							Renderer->Width() * iw::randf() / 2),
				.2f * (iw::randf() + 1.1f)
			));
		}

		m_stars.UpdateParticleMesh();

		return 0;
	}

	void SandLayer::PostUpdate() {	
		if (reset) {
			Reset();
			reset = false;
		}

		int height = m_sandTexture->Height();
		int width  = m_sandTexture->Width();

		vector2 playerLocation = player.Find<iw::Transform>()->Position;
		
		// camera frustrum
		int fy = playerLocation.y - height / 2;
		int fx = playerLocation.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(height - pos.y) + fy;

		//spawnEnemy -= iw::DeltaTime();
		//if (spawnEnemy < 0) {
		//	iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
		//	enemy.Set<iw::Transform>(iw::vector2(1000 * iw::randf()));
		//	enemy.Set<Tile>(std::vector<iw::vector2> {
		//						   vector2(1, 0),				 vector2(3, 0),
		//			vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
		//			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
		//			vector2(0, 3),								 vector2(3, 3),
		//			vector2(0, 4),								 vector2(3, 4)
		//	}, 2);
		//	enemy.Set<Enemy2>(iw::vector2(100 * iw::randf(), 100 * iw::randf()));
		//
		//	spawnEnemy = iw::randf() + 10;
		//}

		Space->Query<iw::Transform, Player, Tile>().Each([&](
			auto,
			auto t,
			auto p,
			auto a)
		{
			if (p->Movement.y == 0) {
				p->Velocity *= .99f;
			}

			p->Velocity = iw::clamp<float>(p->Velocity + p->Movement.y * iw::DeltaTime() * 3, -20, 20);

			vector3 vel = t->Up() * p->Velocity;
			//vel.y = -vel.y;

			float rot = -p->Movement.x * iw::DeltaTime();

			t->Position += vel;
			t->Rotation *= iw::quaternion::from_euler_angles(0, 0, rot);

			p->FireTimeout -= iw::DeltaTime();
			if (   p->FireTimeout < 0
				&& p->FireButtons != 0)
			{
				p->FireTimeout = p->FireButtons.x == 1 ? .05f : .001f;

				float speed      = p->FireButtons.x == 1 ?                                600 :                              1500;
				const Cell& shot = p->FireButtons.x == 1 ? Cell::GetDefault(CellType::BULLET) : Cell::GetDefault(CellType::LASER);

				Fire(t->Position, pos, speed, shot, a->TileId);
			}
		});

		Space->Query<iw::Transform, Enemy2, Tile>().Each([&](
			auto e,
			auto t,
			auto p,
			auto a)
		{
			t->Position = iw::lerp<vector2>(
				t->Position,
				iw::vector2(
					p->Spot.x + cos(iw::TotalTime() + e.Index / 5) * 100,
					p->Spot.y + sin(iw::TotalTime() + e.Index / 5) * 100
				),
				iw::DeltaTime());


			p->FireTimeout -= iw::DeltaTime();
			if (p->FireTimeout < 0) {
				p->FireTimeout = (iw::randf() + 2) * 2;

				Fire(t->Position, playerLocation, 800, Cell::GetDefault(CellType::eLASER), a->TileId);
			}
		});

		if(Keyboard::KeyDown(iw::E) || Keyboard::KeyDown(iw::C) || Keyboard::KeyDown(iw::F) || Keyboard::KeyDown(iw::R)) {
			Cell cell;
			if (Keyboard::KeyDown(iw::C)) {
				cell = Cell::GetDefault(CellType::SAND);
			}

			else if (Keyboard::KeyDown(iw::F)) {
				cell = Cell::GetDefault(CellType::WATER);
			}

			else if (Keyboard::KeyDown(iw::E)) {
				cell = Cell::GetDefault(CellType::EMPTY);
			}

			else if (Keyboard::KeyDown(iw::R)) {
				cell = Cell::GetDefault(CellType::ROCK);
			}
			
			if (Keyboard::KeyDown(iw::G)) {
				cell.Gravitised = true;
			}

			for (int  i = 0; i  < __brushSizeX; i++)
			for (int ii = 0; ii < __brushSizeY; ii++) {
				world.SetCell(pos.x + i, pos.y + ii, cell);
			}
		}

		int updatedCellCount = 0;

		stepTimer -= iw::DeltaTime();
		if (__stepTimeThresh == 0 || (__stepTimeThresh > 0 && stepTimer < 0 && Keyboard::KeyDown(V)/**/)) {
			stepTimer = __stepTimeThresh;
			PasteTiles();
			updatedCellCount = UpdateSandWorld(fx, fy, fx2, fy2);
			UpdateSandTexture(fx, fy, fx2, fy2);
			RemoveTiles();
		}

		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 5000;

		Renderer->BeginScene(MainScene);

			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);

			Renderer->BeforeDraw([&, playerLocation, updatedCellCount]() {
				std::stringstream sb;
				sb << updatedCellCount;
				m_font->UpdateMesh(m_textMesh, sb.str(), 0.001, 1);
			});
			Renderer->DrawMesh(iw::Transform(-.4f), m_textMesh);

		Renderer->EndScene();
	}

	bool SandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };
		player.Find<Player>()->MousePos = mousePos;
		return false;
	}

	bool SandLayer::On(
		MouseButtonEvent& e)
	{
		switch (e.Button) {
			case iw::val(LMOUSE): player.Find<Player>()->FireButtons.x = e.State ? 1 : 0; break;
			case iw::val(RMOUSE): player.Find<Player>()->FireButtons.y = e.State ? 1 : 0; break;
		}

		return false;
	}

	bool SandLayer::On(
		KeyEvent& e)
	{
		Player* p = player.Find<Player>();
		switch (e.Button) {
			case iw::val(D): {
				if (e.State == 1 && p->Movement.x == 1) break;
				p->Movement.x += e.State ? 1 : -1; break;
			}
			case iw::val(A): {
				if (e.State == 1 && p->Movement.x == -1) break;
				p->Movement.x -= e.State ? 1 : -1; break;
			}
							 
			case iw::val(W): {
				if (e.State == 1 && p->Movement.y == 1) break;
				p->Movement.y += e.State ? 1 : -1; break;
			}
			case iw::val(S): {
				if (e.State == 1 && p->Movement.y == -1) break;
				p->Movement.y -= e.State ? 1 : -1; break;
			}
		}

		return false;
	}
	
	void SandLayer::Reset() {
		srand(iw::Ticks());

		world.Reset();
		Space->Clear();

		player = Space->CreateEntity<iw::Transform, Tile, /*SharedCellData,*/ Player>();

		player.Set<iw::Transform>();
		player.Set<Tile>(std::vector<iw::vector2> {
								vector2(1, 0),
				vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
				vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
				vector2(0, 3),							     vector2(3, 3),
				vector2(0, 4),							     vector2(3, 4)
		}, 5);
		//player.Set<SharedCellData>();
		player.Set<Player>();

		// asteroid //

		for (int a = 0; a < 1; a++) {
			float xOff = iw::randf() * a + iw::randf();
			float yOff = iw::randf() * a + iw::randf();

			std::vector<iw::vector2> cells;
			for(int i = -150; i < 150; i++)
			for(int j = -150; j < 150; j++) {
				float x = i;
				float y = j;

				float dist = sqrt(x*x + y*y);

				if (dist < (iw::perlin(x/100 + xOff, y/100 + yOff) + 1) * 75) {
					cells.emplace_back(i, j);
				}
			}

			iw::Entity asteroid = Space->CreateEntity<iw::Transform, Tile, /*SharedCellData,*/ Asteroid>();

			asteroid.Set<iw::Transform>(vector2(iw::randf() * 200, iw::randf() * 200));
			asteroid.Set<Tile>(cells, 1);
			//asteroid.Set<SharedCellData>();
			asteroid.Set<Asteroid>(vector2(iw::randf(), iw::randf())*10);
		}

		// Paste in tiles, should be everyframe and testing for newly spawned tiles

		//Space->Query<iw::Transform, Tile, SharedCellData>().Each([&](
		//	auto e,
		//	auto t,
		//	auto a,
		//	auto s)
		//{
		//	for (int i = 0; i < a->Locations.size(); i++) {
		//		int x = a->Locations[i].x;
		//		int y = a->Locations[i].y;

		//		if (!world.InBounds(x, y)) continue;
		//			
		//		Cell me = Cell::GetDefault(CellType::METAL);
		//		me.Props = CellProperties::MOVE_SHARED_USER;
		//		me.Life = 1;
		//		me.pX = t->Position.x + x;
		//		me.pY = t->Position.y + y;
		//		me.TileId = a->TileId;
		//		me.User = s;

		//		world.SetCell(x, y, me);
		//	}
		//});
	}

	int SandLayer::UpdateSandWorld(
		int fx,  int fy,
		int fx2, int fy2)
	{
		world.Step();

		std::mutex chunkCountMutex;
		std::condition_variable chunkCountCV;
		int chunkCount = 0;
		int cellssUpdatedCount = 0;

		int minX = fx  - world.m_chunkWidth * 2;
		int maxX = fx2 + world.m_chunkWidth * 2;

		int minY = fy  - world.m_chunkHeight * 2;
		int maxY = fy2 + world.m_chunkHeight * 2;

		auto [minCX, minCY] = world.GetChunkLocation(minX, minY); // need to find a way to update all chunks
		auto [maxCX, maxCY] = world.GetChunkLocation(maxX, maxY/*, true*/); // for inclusive / exclusive

		for (int px = 0; px < 2; px++)
		for (int py = 0; py < 2; py++) {
			for (int x = minCX + px; x < maxCX; x += 2)
			for (int y = minCY + py; y < maxCY; y += 2) {
				SandChunk* chunk = world.GetChunkDirect(x, y);
					
				if (!chunk || chunk->IsAllEmpty()) continue;

				{ std::unique_lock lock(chunkCountMutex); chunkCount++; }
				cellssUpdatedCount += chunk->m_filledCellCount;

				Task->queue([&, chunk]() {
					DefaultSandWorker(world, *chunk).UpdateChunk();

					{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
					chunkCountCV.notify_one();
				});
			}

			{
				std::unique_lock lock(chunkCountMutex);
				chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
			}
		}

		for (SandChunk* chunk : world.m_chunks) {
			{ std::unique_lock lock(chunkCountMutex); chunkCount++; }

			Task->queue([&, chunk]() {
				chunk->CommitMovedCells(world.m_currentTick);

				{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
				chunkCountCV.notify_one();
			});
		}

		{
			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

		world.CommitCells();

		return cellssUpdatedCount;
	}

	void SandLayer::UpdateSandTexture(
		int fx,  int fy,
		int fx2, int fy2)
	{
		std::mutex chunkCountMutex;
		std::condition_variable chunkCountCV;
		int chunkCount = 0;

		iw::ref<iw::Texture> sandTex = m_sandTexture;//m_sandScreen.Material()->GetTexture("texture");
			 
		sandTex->Clear();
		unsigned int* colors = (unsigned int*)sandTex->Colors(); // cast from r, g, b, a to rgba

		bool showChunks = Keyboard::KeyDown(K);

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			{ std::unique_lock lock(chunkCountMutex); chunkCount++; }

			Task->queue([=, &chunkCount, &chunkCountCV, &chunkCountMutex]() {
				int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
				int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
				int endY   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
				int endX   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

				for (int y = startY; y < endY; y++)
				for (int x = startX; x < endX; x++) {
					int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * sandTex->Width();

					if (showChunks) {
						const Cell& cell = chunk->GetCellDirect(x, y);

						if (cell.Type != CellType::EMPTY) {
							colors[texi] = cell.Color;
						}

						if (x == 0 || y == 0) {
							colors[texi] = iw::Color(chunk->IsAllEmpty() ? 0 : 1, 0, chunk->IsAllEmpty() ? 1 : 0, 1);
						}

						if (   x == chunk->m_minX || y == chunk->m_minY
							|| x == chunk->m_maxX || y == chunk->m_maxY)
						{
							colors[texi] = iw::Color(0, 1, 0, 1);
						}
					}

					else {
						colors[texi] = chunk->GetCellDirect(x, y).Color;
					}
				}

				{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
				chunkCountCV.notify_one();
			});
		}

		{
			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

		sandTex->Update(Renderer->Device); // should be auto in renderer 
	}
	
	void SandLayer::PasteTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			if (a->Locations.size() < a->InitialLocationsSize / 3) {
				if ((*Space).HasComponent<Player>(e)) {
					reset = true;
				}

				(*Space).QueueEntity(e, iw::func_Destroy);
				return;
			}

			float angle = t->Rotation.euler_angles().z;

			for (int i = 0; i < a->Locations.size(); i++) {
				auto [x, y] = TranslatePoint(a->Locations[i], t->Position, angle);

				if (!world.InBounds(x, y)) continue;
				
				const Cell& cell = world.GetCell(x, y);

				if (   cell.Type   == CellType::EMPTY
					|| cell.TileId == a->TileId
					||(cell.Type == CellType::eLASER && cell.Speed() == 0))
				{
					Cell me = Cell::GetDefault(CellType::METAL);
					me.TileId = a->TileId;
					me.Life = 1;

					world.SetCell(x, y, me);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
	}
	
	void SandLayer::RemoveTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			float angle = t->Rotation.euler_angles().z;

			for (int i = 0; i < a->Locations.size(); i++) {
				auto [x, y] = TranslatePoint(a->Locations[i], t->Position, angle);

				if (!world.InBounds(x, y)) continue;
				
				const Cell& cell = world.GetCell(x, y);

				if (cell.Type == CellType::EMPTY) continue;

				if (cell.TileId == a->TileId) {
					world.SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
	}
}
