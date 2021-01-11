#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"

double __chunkScale = 2;
int __brushSizeX = 50;
int __brushSizeY = 50;
float __stepTime = 1/60.f;
bool __stepDebug = false;

namespace iw {
	SandLayer::SandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(200, 200, __chunkScale))
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

		Cell empty   = { CellType::EMPTY };
		Cell sand    = { CellType::SAND,  CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };
		Cell water   = { CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE };
		Cell rock    = { CellType::ROCK  };
		Cell metal   = { CellType::METAL };
		Cell debris  = { CellType::DEBRIS };
		Cell smoke   = { CellType::SMOKE,     CellProperties::MOVE_RANDOM | CellProperties::DELETE_TIME };
		Cell explosn = { CellType::EXPLOSION, CellProperties::MOVE_RANDOM | CellProperties::DELETE_TIME };
		
		Cell laser   = { CellType::LASER,   CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_BEAM };
		Cell elaser  = { CellType::eLASER,  CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_BEAM };
		Cell bullet  = { CellType::BULLET,  CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_PROJECTILE };
		Cell missile = { CellType::MISSILE, CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_MISSILE };

		empty  .Color = iw::Color::From255(  0,   0,   0, 0);
		sand   .Color = iw::Color::From255(237, 201, 175);
		water  .Color = iw::Color::From255(175, 201, 237);
		rock   .Color = iw::Color::From255(201, 201, 201);
		metal  .Color = iw::Color::From255(230, 230, 230);
		debris .Color = iw::Color::From255(150, 150, 150);
		explosn.Color = iw::Color::From255(255,  66,  33);
		smoke  .Color = explosn.Color; //lerped to -> iw::Color::From255(100, 100, 100);
		laser  .Color = iw::Color::From255(255,   0,   0);
		elaser .Color = iw::Color::From255(  0, 200, 255);
		bullet .Color = iw::Color::From255(255, 255,   0);
		missile.Color = metal.Color;

		sand .dY = -1;
		water.dY = -1;
		smoke.dX = smoke.dY = .5f;
		explosn.dX = explosn.dY = 2;

		bullet.dX  = 500; // Initial speeds
		laser.dX   = 1500;
		elaser.dX  = 1000;
		missile.dX = 200;

		laser .Life  = 0.06f;
		elaser.Life  = 1.00f;
		bullet.Life  = 0.01f;
		missile.Life = 0.05f;
		explosn.Life = 0.06f;
		smoke.Life   = 0.5f;

		empty  .Precedence = 0;
		smoke  .Precedence = 1;
		explosn.Precedence = 100;
		laser  .Precedence = 15;
		elaser .Precedence = 15;
		bullet .Precedence = 15;
		missile.Precedence = 101;

		Cell::SetDefault(CellType::EMPTY,     empty);
		Cell::SetDefault(CellType::SAND,      sand);
		Cell::SetDefault(CellType::WATER,     water);
		Cell::SetDefault(CellType::ROCK,      rock);
		Cell::SetDefault(CellType::METAL,     metal);
		Cell::SetDefault(CellType::DEBRIS,    debris);
		Cell::SetDefault(CellType::SMOKE,     smoke);
		Cell::SetDefault(CellType::EXPLOSION, explosn);

		Cell::SetDefault(CellType::LASER,     laser);
		Cell::SetDefault(CellType::eLASER,    elaser);
		Cell::SetDefault(CellType::BULLET,    bullet);
		Cell::SetDefault(CellType::MISSILE,   missile);

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

		float deltaTime = iw::DeltaTime() > __stepTime ? iw::DeltaTime() : __stepTime;

		int height = m_sandTexture->Height();
		int width  = m_sandTexture->Width();

		vector2 playerLocation = player ? player.Find<iw::Transform>()->Position : 0;
		
		// camera frustrum
		int fy = playerLocation.y - height / 2;
		int fx = playerLocation.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(height - pos.y) + fy;

		if(    Keyboard::KeyDown(iw::E)
			|| Keyboard::KeyDown(iw::C) 
			|| Keyboard::KeyDown(iw::F) 
			|| Keyboard::KeyDown(iw::R)
			|| Keyboard::KeyDown(iw::Q))
		{
			Cell cell;
			if (Keyboard::KeyDown(iw::C)) {
				cell = Cell::GetDefault(CellType::SAND);
			}

			else if (Keyboard::KeyDown(iw::F)) {
				cell = Cell::GetDefault(CellType::WATER);
			}

			else if (Keyboard::KeyDown(iw::Q)) {
				cell = Cell::GetDefault(CellType::SMOKE);
				cell.Life = 10000;
				cell.dX = 10;
				cell.dY = 10;
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
				cell.pX = pos.x + i;
				cell.pY = pos.y + ii;
				world.SetCell(pos.x + i, pos.y + ii, cell);
			}
		}

		stepTimer -=  iw::DeltaTime();
		if (stepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) {
			Space->Query<iw::Transform, Player, Tile>().Each([&](
				auto,
				auto t,
				auto p,
				auto a)
			{
				if (p->Movement.y == 0) {
					p->Velocity *= .99f;
				}

				p->Velocity = iw::clamp<float>(p->Velocity + p->Movement.y, -100, 100);

				vector3 vel = t->Up() * p->Velocity *  deltaTime * 3;
				//vel.y = -vel.y;

				float rot = -p->Movement.x *  deltaTime;

				t->Position += vel;
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, rot);
				p->FireTimeout -= deltaTime;

				if (   p->FireTimeout < 0
					&& p->FireButtons != 0)
				{
					if (p->FireButtons.x == 1) {
						p->FireTimeout = 0.035f;
						Fire(t->Position, pos, vel, Cell::GetDefault(CellType::BULLET), a->TileId, true);
					}

					else if (p->FireButtons.y == 1) {
						p->FireTimeout = 0.001f;
						Fire(t->Position, pos, 0, Cell::GetDefault(CellType::LASER), a->TileId, false);
					}

					else if (p->FireButtons.z == 1) {
						p->FireTimeout = 0.01f;
						vector2 d = t->Position + t->Right() * (iw::randf() > 0 ? 1 : -1);
						FireMissile(t->Position, d, Cell::GetDefault(CellType::MISSILE), a->TileId);
					}
				}
			});

			Space->Query<iw::Transform, EnemyBase, Tile>().Each([&](
				auto, 
				auto t,
				auto b, 
				auto a)
			{
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, deltaTime/60);

				b->Energy += deltaTime;

				float enemy = iw::randf() + 3;
				float group = enemy * 10 + 10;

				while (b->Energy > b->NextGroup) {
					b->Energy -= b->EnemyCost;

					iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
					enemy.Set<iw::Transform>(t->Position + 200*iw::vector2(iw::randf(), iw::randf()));
					enemy.Set<Tile>(std::vector<iw::vector2> {
						vector2(1, 0), vector2(3, 0),
							vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
							vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
							vector2(0, 3), vector2(3, 3),
							vector2(0, 4), vector2(3, 4)
					}, 3);
					enemy.Set<Enemy2>(iw::vector2(100 * iw::randf(), 100 * iw::randf()));
				}
			
				b->FireTimer -= deltaTime;

				if (b->FireTimer > 0) {
					Fire(t->Position + (playerLocation - t->Position).normalized() * 100, 
						playerLocation, 0, Cell::GetDefault(CellType::LASER), a->TileId, false);
				}

				else if (b->FireTimer < -b->FireTimeout) {
					b->FireTimer = b->FireTime + iw::randf();
				}
			});

			Space->Query<iw::Transform, Enemy2, Tile>().Each([&](
				auto e,
				auto t,
				auto p,
				auto a)
			{
				p->Spot.x = cos(iw::TotalTime() + e.Index / 5) * 100;
				p->Spot.y = sin(iw::TotalTime() + e.Index / 5) * 100;

				iw::vector2 nD = (p->Spot - t->Position).normalized();
				iw::vector2 delta = (nD - p->Vel) * p->Vel.length() * p->TurnRad;

				p->Vel = (p->Vel + delta).normalized() * p->Vel.length();

				t->Position += p->Vel;

				p->FireTimeout -= deltaTime;
				if (p->FireTimeout < 0) {
					p->FireTimeout = (iw::randf() + 2) * 2;

					Fire(t->Position, playerLocation, 0, Cell::GetDefault(CellType::eLASER), a->TileId, true);
				}
			});

			auto space = Space; // :<

			Space->Query<Missile, SharedCellData>().Each([&, space](
				auto e,
				auto m,
				auto s)
			{
				s->Timer += deltaTime;

				if (s->UserCount == 0) {
					space->QueueEntity(e, iw::func_Destroy);
					return;
				}

				if (   s->Timer > m->WaitTime
					&& s->Timer < m->WaitTime + m->BurnTime)
				{
					iw::vector2 mpos(s->pX, s->pY);
					iw::vector2 mvel(s->vX, s->vY);
				
					iw::vector2 nV = mvel.normalized();

					iw::vector2 closest;
					float minDist = FLT_MAX;
					space->Query<iw::Transform, Enemy2>().Each([&](
						auto, auto t2, auto)
					{
						iw::vector2 v = t2->Position - mpos;
						if (v.length_squared() < minDist) {
							minDist = v.length_squared();
							closest = t2->Position;
						}
					});

					if (minDist != FLT_MAX) {
						if (minDist < 800) {
							s->Hit = true; // explode
							return;
						}
					
						iw::vector2 nD = (closest - mpos).normalized();
						iw::vector2 delta = (nD - nV) * s->Speed * m->TurnRad;

						mvel = (mvel + delta).normalized()* s->Speed;
					
						s->vX = mvel.x;
						s->vY = mvel.y;
					}
				
					s->Speed = iw::clamp<float>(s->Speed * (1 + deltaTime * 3),
						Cell::GetDefault(CellType::MISSILE).Speed(),
						Cell::GetDefault(CellType::MISSILE).Speed()*2
					);

					// Spawn smoke

					iw::vector2 spos = mpos - mvel * deltaTime * 4;

					Cell smoke = Cell::GetDefault(CellType::SMOKE);
					smoke.TileId = m->TileId;
					smoke.pX = spos.x;
					smoke.pY = spos.y;
					smoke.Life = 10 + iw::randf() * 3;

					// normalize speed ?

					world.SetCell(smoke.pX, smoke.pY, smoke);
				}

				if (s->Hit) {
					iw::vector2 v(s->pX - s->hX, s->pY - s->hY);
					v.normalize();

					s->vX += v.x * (iw::randf() + 1)/5;
					s->vY += v.y * (iw::randf() + 1)/5;
					s->Hit = false;
				}
			});

			stepTimer = deltaTime;
			PasteTiles();
			UpdateSharedUserData(deltaTime);
			UpdateSandWorld(fx, fy, fx2, fy2, deltaTime);
			UpdateSandTexture(fx, fy, fx2, fy2);
			RemoveTiles();
		}

		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 5000;

		Renderer->BeginScene(MainScene);

			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);

			Renderer->BeforeDraw([&, playerLocation]() {
				std::stringstream sb;
				sb << 1 / deltaTime;
				m_font->UpdateMesh(m_textMesh, sb.str(), 0.001, 1);
			});
			Renderer->DrawMesh(iw::Transform(-.4f), m_textMesh);

		Renderer->EndScene();
	}

	bool SandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };

		if (!player) return false;
		player.Find<Player>()->MousePos = mousePos;

		return false;
	}

	bool SandLayer::On(
		MouseButtonEvent& e)
	{
		if (!player) return false;

		Player* p = player.Find<Player>();
		switch (e.Button) {
			case iw::val(LMOUSE): p->FireButtons.x = e.State ? 1 : 0; break;
			case iw::val(RMOUSE): p->FireButtons.y = e.State ? 1 : 0; break;
		}

		return false;
	}

	bool SandLayer::On(
		KeyEvent& e)
	{
		if (!player) return false;

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
			case iw::val(SPACE): {
				p->FireButtons.z = e.State ? 1 : -1; break;
			}
		}

		return false;
	}
	
	void SandLayer::Reset() {
		world.Reset();
		Space->Clear();

		player = Space->CreateEntity<iw::Transform, Tile, Player>();

		player.Set<iw::Transform>();
		player.Set<Tile>(std::vector<iw::vector2> {
				               vector2(1, 4),
				vector2(0, 3), vector2(1, 3), vector2(2, 3), vector2(3, 3),
				vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
				vector2(0, 1),							     vector2(3, 1),
				vector2(0, 0),							     vector2(3, 0)
		}, 5);
		player.Set<Player>();

		//// enemy base //

		iw::Entity e = Space->CreateEntity<iw::Transform, EnemyBase, Tile>();
		e.Set<iw::Transform>(iw::vector2(1000, 0));
		e.Set<EnemyBase>();
		e.Set<Tile>(std::vector<iw::vector2> {
				               vector2(1, 2),
				vector2(0, 1), vector2(1, 1), vector2(2, 1),
				               vector2(1, 0),                
		}, 100);


		// asteroid //

		srand(iw::Ticks());

		for (int a = 0; a < 3; a++) {
			iw::Entity asteroid = Space->CreateEntity<iw::Transform, Tile>();

			iw::Transform* t = asteroid.Set<iw::Transform>(vector2(iw::randf() * 200, iw::randf() * 200));
			Tile*          d = asteroid.Set<Tile>();
			
			float xOff = (2+a) * 100 + iw::randf() * 50;
			float yOff = (2+a) * 100 + iw::randf() * 50;

			int count = 0;
			for(int i = -150; i < 150; i++)
			for(int j = -150; j < 150; j++) {
				float x = i;
				float y = j;

				float dist = sqrt(x*x + y*y);

				if (dist < (iw::perlin(x/70 + xOff, y/70 + yOff) + 1) * 75) {
					d->Locations.emplace_back(x + xOff, y + yOff);
				}
			}
		}

		/*for(int x =  15; x < 30; x++)
		for(int y = -10; y < 10; y++) {
			world.SetCell(x, y, Cell::GetDefault(CellType::ROCK));
		}*/
	}

	int SandLayer::UpdateSandWorld(
		int fx,  int fy,
		int fx2, int fy2,
		float deltaTime)
	{
		//if (Mouse::ButtonDown(iw::LMOUSE)) {
		//	Cell c = Cell::GetDefault(CellType::BULLET);
		//	c.pX = -50; c.pY = 0;
		//	world.SetCell(-50, 0, c);
		//}

		world.Step();

		std::mutex chunkCountMutex;
		std::condition_variable chunkCountCV;
		int chunkCount = 0;
		int cellssUpdatedCount = 0;

		int minX = fx  - world.m_chunkWidth * 10;
		int maxX = fx2 + world.m_chunkWidth * 10;

		int minY = fy  - world.m_chunkHeight * 10;
		int maxY = fy2 + world.m_chunkHeight * 10;

		auto [minCX, minCY] = world.GetChunkLocation(minX, minY);
		auto [maxCX, maxCY] = world.GetChunkLocation(maxX, maxY);

		for (int px = 0; px < 2; px++)
		for (int py = 0; py < 2; py++) {
			for (int x = minCX + px; x < maxCX; x += 2)
			for (int y = minCY + py; y < maxCY; y += 2) {
				SandChunk* chunk = world.GetChunkDirect(x, y);
					
				if (!chunk || chunk->IsAllEmpty()) continue;

				{ std::unique_lock lock(chunkCountMutex); chunkCount++; }
				cellssUpdatedCount += chunk->m_filledCellCount;

				Task->queue([&, chunk]() {
					DefaultSandWorker(world, *chunk, Space, deltaTime).UpdateChunk();

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
	
	void SandLayer::UpdateSharedUserData(
		float deltaTime)
	{
		Space->Query<SharedCellData>().Each([&](
			auto e,
			auto s)
		{
			s->pX += s->vX * deltaTime;
			s->pY += s->vY * deltaTime;

			//s->angle += deltaTime;
		});
	}

	void SandLayer::PasteTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			float angle = t->Rotation.euler_angles().z;

			SandChunk* chunk = nullptr;

			for (int i = 0; i < a->Locations.size(); i++) {
				auto [x, y] = TranslatePoint(a->Locations[i], t->Position, angle);

				if (!chunk || !chunk->InBounds(x, y)) {
					chunk = world.GetChunk(x, y);
				}

				const Cell& cell = chunk->GetCell(x, y);

				if (   cell.Type   == CellType::EMPTY
					|| cell.TileId == a->TileId
					|| cell.Precedence < a->Precedence)
				{
					Cell me = Cell::GetDefault(CellType::METAL);
					me.TileId = a->TileId;
					me.Life = 1;

					chunk->SetCell(x, y, me, world.m_currentTick);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}

			if (a->Locations.size() < a->InitialLocationsSize / 3) {
				if ((*Space).HasComponent<Player>(e)) {
					reset = true;
				}

				(*Space).QueueEntity(e, iw::func_Destroy);
				return;
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

			SandChunk* chunk = nullptr;

			for (int i = 0; i < a->Locations.size(); i++) {
				auto [x, y] = TranslatePoint(a->Locations[i], t->Position, angle);

				if (!chunk || !chunk->InBounds(x, y)) {
					chunk = world.GetChunk(x, y);
				}
				
				const Cell& cell = chunk->GetCell(x, y);

				if (cell.Type == CellType::EMPTY) continue;

				if (   cell.TileId == a->TileId
					|| cell.Precedence < a->Precedence)
				{
					chunk->SetCell(x, y, Cell::GetDefault(CellType::EMPTY), world.m_currentTick);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
	}
}
