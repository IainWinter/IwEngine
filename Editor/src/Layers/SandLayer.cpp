#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/math/matrix.h"

std::unordered_map<CellType, Cell> Cell::m_defaults = {};

namespace iw {
	SandLayer::SandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(200, 200, 2))
		, reset(true)
	{
		srand(time(nullptr));
	}

	int SandLayer::Initialize() {
		Cell empty  = { CellType::EMPTY };
		Cell sand   = { CellType::SAND,  CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };
		Cell water  = { CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE };
		Cell rock   = { CellType::ROCK  };
		Cell metal  = { CellType::METAL };
		Cell debris = { CellType::DEBRIS };
		Cell laser  = { CellType::LASER,  CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell elaser = { CellType::eLASER, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_BEAM };
		Cell bullet = { CellType::BULLET, CellProperties::MOVE_FORWARD | CellProperties::HIT_LIKE_PROJECTILE };

		empty  .Color = iw::Color::From255(  0,   0,   0);
		sand   .Color = iw::Color::From255(237, 201, 175);
		water  .Color = iw::Color::From255(175, 201, 237);
		rock   .Color = iw::Color::From255(201, 201, 201);
		metal  .Color = iw::Color::From255(230, 230, 230);
		debris .Color = iw::Color::From255(150, 150, 150);
		laser  .Color = iw::Color::From255(255,   0,   0);
		elaser .Color = iw::Color::From255(  0, 200, 255);
		bullet .Color = iw::Color::From255(255, 255,   0);

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

		if (int err = Layer::Initialize()) { 
			return err;
		}

		// Sand

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

		//iw::ref<iw::Texture> starTexture = REF<iw::Texture>(1, 1, iw::TEX_2D, iw::RGBA);
		//starTexture->SetFilter(iw::NEAREST);
		//starTexture->CreateColors();
		//starTexture->Clear();

//		((unsigned int*)starTexture->Colors())[50] = INT_MAX;

		//int i = starTexture->ColorCount() / 2 - 2;
		//starTexture->Colors()[i  ] = 255;
		//starTexture->Colors()[i+1] = 255;
		//starTexture->Colors()[i+2] = 255;
		//starTexture->Colors()[i+3] = 255;

		/*for (unsigned char* c = starTexture->Colors(); c != starTexture->Colors() + starTexture->ColorCount(); c++) {
			*c = 255;
		}*/

		iw::ref<iw::Material> starMat = REF<iw::Material>(starsShader);
		//starMat->SetTexture("texture", starTexture);

		starMat->Set("color", iw::Color(1));

		iw::Mesh starMesh = Renderer->ScreenQuad().MakeCopy();

		iw::vector3* p  = (iw::vector3*)starMesh.Data()->Get(bName::POSITION);
		iw::vector2* u  = (iw::vector2*)starMesh.Data()->Get(bName::UV);
		unsigned int* i = starMesh.Data()->GetIndex();

		starMesh.Data()->SetBufferData(bName::POSITION, 1, p);
		starMesh.Data()->SetBufferData(bName::UV,       1, u);
		starMesh.Data()->SetIndexData(1, i);

		starMesh.SetMaterial(starMat);

		starMesh.Data()->SetTopology(iw::MeshTopology::POINTS);

		m_stars.SetParticleMesh(starMesh);

		m_stars.SetTransform(new iw::Transform(0, .1f));

		for (int i = 0; i < 2000; i++) {
			m_stars.SpawnParticle(iw::Transform(
				iw::vector2(Renderer->Width() * iw::randf() / 2,
							Renderer->Width() * iw::randf() / 2)));
		}

		m_stars.UpdateParticleMesh();

		return 0;
	}

	void SandLayer::PostUpdate() {
		// Game update

		world.m_currentTick += 1;

		int height = m_sandTexture->Height();
		int width  = m_sandTexture->Width();

		if (reset) {
			world.Reset();
			Space->Clear();

			player = Space->CreateEntity<iw::Transform, Tile, Player>();

			player.Set<iw::Transform>();
			player.Set<Tile>(std::vector<iw::vector2> {
								   vector2(1, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3),							     vector2(3, 3),
					vector2(0, 4),							     vector2(3, 4)
			}, -5);
			player.Set<Player>();

			//for (int i = 1; i < m_sandTexture->Width(); i++) {
			//	world.SetCell(i-width/2, 0, Cell::GetDefault(CellType::ROCK));
			//}

			//for (int i = 1; i < m_sandTexture->Width(); i++) {
			//	world.SetCell(i-width/2, m_sandTexture->Height()-1, Cell::GetDefault(CellType::ROCK));
			//}

			//for (int i = 1; i < m_sandTexture->Height(); i++) {
			//	world.SetCell(0, i-height/2, Cell::GetDefault(CellType::ROCK));
			//}

			//for (int i = 1; i < m_sandTexture->Height(); i++) {
			//	world.SetCell(m_sandTexture->Width() - 1, i-height/2, Cell::GetDefault(CellType::ROCK));
			//}

			reset = false;
		}

		spawnEnemy -= iw::DeltaTime();
		if (spawnEnemy < 0) {
			iw::Entity enemy = Space->CreateEntity<iw::Transform, Tile, Enemy2>();
			enemy.Set<iw::Transform>(iw::vector2(1000 * iw::randf()));
			enemy.Set<Tile>(std::vector<iw::vector2> {
								   vector2(1, 0),				 vector2(3, 0),
					vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
					vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
					vector2(0, 3),								 vector2(3, 3),
					vector2(0, 4),								 vector2(3, 4)
			}, 2);
			enemy.Set<Enemy2>(iw::vector2(100 * iw::randf(), 100 * iw::randf()));

			spawnEnemy = iw::randf() + 10;
		}

		// camera frustrum, mostly used at the bottom, but needed for screen mouse pos to world pos
		vector2 playerLocation = player.Find<iw::Transform>()->Position;
		
		int fy = playerLocation.y - height / 2;
		int fx = playerLocation.x - width  / 2;
		int fy2 = fy + height;
		int fx2 = fx + width;

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(height - pos.y) + fy;
		
		gravPos = pos;

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
			
			for (int  i = 0; i  < 20; i++)
			for (int ii = 0; ii < 20; ii++) {
				world.SetCell(pos.x + i, pos.y + ii, cell);
			}
		}

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

			for (int i = 0; i < a->Locations.size(); i++) {
				vector2 v = vector4(a->Locations[i], 0, 1) * t->Transformation().transposed();

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

				if (   cell.Type   == CellType::EMPTY
					|| cell.TileId == a->TileId
					||(cell.Type == CellType::eLASER && cell.Speed() == 0))
				{
					Cell me = Cell::GetDefault(CellType::METAL);
					me.TileId = a->TileId;
					me.Life = 1;

					world.SetCell(v.x, v.y, me);
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});

		Space->Query<iw::Transform, Tile, Player>().Each([&](
			auto,
			auto t,
			auto a,
			auto p)
		{
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

		Space->Query<iw::Transform, Tile, Enemy2>().Each([&](
			auto,
			auto t,
			auto a,
			auto p)
		{
			p->FireTimeout -= iw::DeltaTime();

			if (p->FireTimeout < 0) {
				p->FireTimeout = (iw::randf() + 2) * 2;

				Fire(t->Position, playerLocation, 800, Cell::GetDefault(CellType::eLASER), a->TileId);
			}
		});

		// Sand update

		//stepTimer -= iw::DeltaTime();
		//if (stepTimer < 0 /*&& Keyboard::KeyDown(V)*/) {
		//	stepTimer = 1 / 180.0f;

			std::mutex chunkCountMutex;
			std::condition_variable chunkCountCV;
			int chunkCount = 0;

			for(auto& pair: world.m_chunks) {
				if (pair.second.IsEmpty()) {
					continue;
				}

				if ((iw::vector2(pair.second.m_x, pair.second.m_y) - playerLocation).length() > fx2 - fx) {
					continue;
				}

				{
					std::unique_lock lock(chunkCountMutex);
					chunkCount++;
				}

				Task->queue([&]() {
					SandWorker(world, pair.second).UpdateChunk();

					{
						std::unique_lock lock(chunkCountMutex);
						chunkCount--;
					}

					chunkCountCV.notify_one();
				});
			}

			//LOG_INFO << "Need to update " << chunkCount << " chunks. dt is " << iw::DeltaTime() << " seconds (" << 1 / iw::DeltaTime() << "fps)";

			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&](){ return chunkCount == 0; });
		
		//}

		// Swap buffers


		iw::ref<iw::Texture> sandTex = m_sandTexture;//m_sandScreen.Material()->GetTexture("texture");
			 
		sandTex->Clear();
		unsigned int* colors = (unsigned int*)sandTex->Colors(); // cast from r, g, b, a to rgba

		//m_stars.seed(1);

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
			int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
			int endY   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
			int endX   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

			for (int y = startY; y < endY; y++)
			for (int x = startX; x < endX; x++) {
				int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * width;

#undef max

				//if (m_stars() < m_stars.max() / 2000) {
				//	colors[texi] = INT_MAX;
				//}

				//const Cell& cell = chunk->GetCellUnsafe(x, y);

				//if (cell.Type != CellType::EMPTY) {
					colors[texi] = chunk->GetCellUnsafe(x, y).Color;
				//}

				//else {
				//}

				if (x == 0 || y == 0) {
					colors[texi] = iw::Color(1, 0, 0, 1);
				}

				/*
				if (Keyboard::KeyDown(K)) {
					if (x == 0 || y == 0) {
						colors[texi] = iw::Color(1, 0, 0, 1);
					}
				}
				*/
			}
		}

		sandTex->Update(Renderer->Device); // should be auto in renderer 


		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 10000;


		Renderer->BeginScene(MainScene);
			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);
		Renderer->EndScene();

		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			for (int i = 0; i < a->Locations.size(); i++) {
				vector2 v = vector4(a->Locations[i], 0, 1) * t->Transformation().transposed();

				if (!world.InBounds(v.x, v.y)) continue;
				
				const Cell& cell = world.GetCell(v.x, v.y);

				if (cell.Type == CellType::EMPTY) continue;

				if (cell.TileId == a->TileId) {
					world.SetCell(v.x, v.y, Cell::GetDefault(CellType::EMPTY));
				}

				else {
					a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					i--;
				}
			}
		});
		
		Space->Query<iw::Transform, Player>().Each([&](
			auto,
			auto t,
			auto p)
		{
			p->Velocity *= 1 - iw::DeltaTime();
			p->Velocity = iw::clamp<float>(p->Velocity + p->Movement.y * iw::DeltaTime() * 3, -15, 15);

			t->Position += t->Up() * p->Velocity;
			t->Rotation *= iw::quaternion::from_euler_angles(0, 0, -p->Movement.x * iw::DeltaTime());

			//if (   t->Position.x < -world.m_chunkWidth  * 10 + world.m_chunkWidth
			//	|| t->Position.y < -world.m_chunkHeight * 10 + world.m_chunkHeight
			//	|| t->Position.x >  world.m_chunkWidth  * 10 - world.m_chunkWidth
			//	|| t->Position.y >  world.m_chunkHeight * 10 - world.m_chunkHeight)
			//{
			//	p->Velocity -= iw::DeltaTime();
			//}
		});

		Space->Query<iw::Transform, Enemy2>().Each([&](
			auto e,
			auto t,
			auto p)
		{
			t->Position = iw::lerp<vector2>(
				t->Position,
				iw::vector2(
					p->Spot.x + cos(iw::TotalTime() + e.Index / 5) * 100,
					p->Spot.y + sin(iw::TotalTime() + e.Index / 5) * 100
				),
				iw::DeltaTime());
		});

		//Renderer->ApplyFilter(m_shader, m_target, nullptr);
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

	std::vector<iw::vector2> FillLine(
		int x,  int y,
		int x2, int y2)
	{
		std::vector<iw::vector2> positions; // https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm

		int dx =  abs(x2 - x);
		int dy = -abs(y2 - y);
		int sx = x < x2 ? 1 : -1;
		int sy = y < y2 ? 1 : -1;
		int err = dx + dy;  /* error value e_xy */
		
		while (true) {
			positions.emplace_back(x, y);

			if (x == x2 && y == y2) break;
			
			int e2 = 2 * err;
			if (e2 >= dy) { /* e_xy + e_x > 0 */
				err += dy;
				x += sx;
			}

			if (e2 <= dx) { /* e_xy + e_y < 0 */
				err += dx;
				y += sy;
			}
		}

		return positions;
	}
}

void SandWorker::UpdateChunk() {
	for (int x = 0;                  x <  m_chunk.m_width; x++)
	for (int y = m_chunk.m_height-1; y >= 0;               y--) {
		
		if (CellAlreadyUpdated(x, y)) continue;

		Cell cell = m_chunk.GetCell(x, y);

		// Would go in grav worker or something

		//if (cell.Gravitised) {
		//	vector2 dir = gravPos - vector2(x, y);

		//	float div = 2;
		//	while (abs((dir / div).major()) > 1) {
		//		dir /= div;
		//		div = iw::clamp(div - iw::randf(), 1.2f, 2.f);
		//	}

		//	Cell& scell = chunk.GetCell(x, y, true);

		//	cell.Direction  += dir + iw::DeltaTime();
		//	scell.Direction += dir + iw::DeltaTime();

		//	cell.Direction  *= .9f;
		//	scell.Direction *= .9f;

		//	cell.Direction  = iw::clamp<vector2>(cell.Direction, -5, 5);
		//	scell.Direction = iw::clamp<vector2>(scell.Direction, -5, 5);
		//}

		const Cell& replacement = Cell::GetDefault(cell.Type);

		bool moved = true;
		
		bool hit = false;
		int hitx, hity;

		if		(cell.Props & CellProperties::MOVE_DOWN      && MoveDown    (x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_DOWN_SIDE && MoveDownSide(x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_SIDE      && MoveSide    (x, y, cell, replacement)) {}
		else if (cell.Props & CellProperties::MOVE_FORWARD   && MoveForward (x, y, cell, replacement, hit, hitx, hity)) {}
		else {
			moved = false;
		}
		
		if (moved) {
			SetCell(x, y, Cell::GetDefault(CellType::EMPTY));
		}

		if (!hit) {
			continue;
		}

		if		(cell.Props & CellProperties::HIT_LIKE_BEAM)	   { HitLikeBeam(hitx, hity, x, y, cell); }
		else if (cell.Props & CellProperties::HIT_LIKE_PROJECTILE) { HitLikeProj(hitx, hity, x, y, cell); }
	}
}

bool SandWorker::MoveDown(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int downX = x + cell.dX;
	int downY = y + cell.dY;

	bool down = IsEmpty(downX, downY);
	if (down) {
		SetCell(downX, downY, replace);
	}

	return down;
}

bool SandWorker::MoveDownSide(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int downLeftX = x + cell.dY + cell.dX;
	int downLeftY = y - cell.dX + cell.dY;

	int downRightX = x - cell.dY + cell.dX;
	int downRightY = y + cell.dX + cell.dY;

	bool downLeft  = IsEmpty(downLeftX,  downLeftY);
	bool downRight = IsEmpty(downRightX, downRightY);

	if (downLeft && downRight) {
		bool rand = iw::randf() > 0;
		downLeft  = rand ? true  : false;
		downRight = rand ? false : true;
	}

	if		(downLeft)	SetCell(downLeftX,  downLeftY,  replace);
	else if (downRight) SetCell(downRightX, downRightY, replace);

	return downLeft || downRight;
}

bool SandWorker::MoveSide(
	int x, int y,
	const Cell& cell,
	const Cell& replace)
{
	int leftX = x + cell.dY + cell.dX;
	int leftY = y - cell.dX;

	int rightX = x - cell.dY + cell.dX;
	int rightY = y + cell.dX;

	bool left  = IsEmpty(leftX,  leftY);
	bool right = IsEmpty(rightX, rightY);

	if (left && right) {
		bool rand = iw::randf() > 0;
		left  = rand ? true  : false;
		right = rand ? false : true;
	}

	if		(left)	SetCell(leftX,	leftY,	replace);
	else if (right) SetCell(rightX, rightY, replace);

	return left || right;
}

bool SandWorker::MoveForward(
	int x, int y,
	const Cell& cell,
	const Cell& replace,
	bool& hit, int& hitx, int& hity)
{
	float life = cell.Life - iw::DeltaTime();

	if (life < 0) {
		return true;
	}

	else {
		std::vector<iw::vector2> cellpos = iw::FillLine(x, y, x + cell.dX * iw::DeltaTime(), y + cell.dY * iw::DeltaTime());
		for (int i = 0; i < cellpos.size(); i++) {
			float destX = cellpos[i].x;
			float destY = cellpos[i].y;

			bool forward = IsEmpty(destX, destY);

			if (forward) {
				bool last = i == (cellpos.size() - 1);

				Cell replacement = replace;
				replacement.TileId = cell.TileId;
				replacement.dX = last ? cell.dX : 0;
				replacement.dY = last ? cell.dY : 0;

				SetCell(destX, destY, replacement);
			}

			else if (InBounds(destX, destY)
				  && GetCell (destX, destY).TileId != cell.TileId)
			{
				hit = true;
				hitx = destX;
				hity = destY;
				break;
			}
		}

		Cell replacement = cell;
		replacement.Life = life;
		replacement.dX = 0;
		replacement.dY = 0;

		SetCell(x, y, replacement);
	}

	return false;
}

void SandWorker::HitLikeProj(
	int x,  int y,
	int lx, int ly,
	const Cell& bullet)
{
	float speed = bullet.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	if (hit.Life < 0) {
		Cell cell = bullet;
		cell.dX = iw::clamp<int>(cell.dX + iw::randi(100) - 50, cell.dX * .8 - 50, cell.dX * 1.2 + 50);
		cell.dY = iw::clamp<int>(cell.dY + iw::randi(100) - 50, cell.dY * .8 - 50, cell.dY * 1.2 + 50);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

void SandWorker::HitLikeBeam(
	int x,  int y,
	int lx, int ly,
	const Cell& laser)
{
	float speed = laser.Speed();

	Cell hit = GetCell(x, y);
	hit.Life -= speed;

	if (   hit.Type == CellType::LASER 
		|| hit.Type == CellType::eLASER)
	{
		SetCell(lx + laser.dX, ly + laser.dY, laser);
		SetCell(lx, ly, Cell::GetDefault(CellType::EMPTY));

		return;
	}

	if (hit.Life <= 0) {
		Cell cell = laser;
		cell.dX = iw::clamp<int>(cell.dX + iw::randi(100) - 50, cell.dX * .8 - 50, cell.dX * 1.2 + 50);
		cell.dY = iw::clamp<int>(cell.dY + iw::randi(100) - 50, cell.dY * .8 - 50, cell.dY * 1.2 + 50);
		cell.Life *= 1 - iw::DeltaTime() * 5;

		SetCell(x, y, cell);
	}

	else {
		SetCell(x, y, hit);
	}
}

//for (int i = x - 5; i < x + 5; i++)
//for (int j = y - 5; j < y + 5; j++) {
//	if (   inBounds(i, j)
//		&& getCell(i, j).Type != cell.Type)
//	{
//		if (iw::randf() > .5) {
//			setCell(i, j, _DEBRIS);
//		}
//
//		else {
//			setCell(i, j, _EMPTY);
//		}
//	}
//}
