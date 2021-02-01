#include "Layers/FactorySandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"
#include "iw/common/algos/MarchingCubes.h"

#include "iw/physics/Collision/MeshCollider.h"

double __chunkScale = 4.f;
int __chunkSize = 200;
int __arenaSize = 50;
int __arenaSizeBuf = 1;
int __brushSizeX = 5;
int __brushSizeY = 5;
float __stepTime = 1/60.f;
bool __stepDebug = false;

int __worldSize = __arenaSize * __chunkSize / __chunkScale; // gets set below

int __worldSizeX = 1;
int __worldSizeY = 5;

int __seaHeight = 125;

//#define _DEBUG 1

namespace iw {
	FactorySandLayer::FactorySandLayer()
		: Layer("Factory Sand")
		, m_sandTexture(nullptr)
		, world(SandWorld(__chunkSize, __chunkSize, __chunkScale))
		, reset(true)
	{
		world.SetMaxChunks(__worldSizeX, __worldSizeY);

		__worldSizeX *= __chunkSize / __chunkScale;
		__worldSizeY *= __chunkSize / __chunkScale;
	}

	int FactorySandLayer::Initialize() {
		if (int err = Layer::Initialize()) {
			return err;
		}

		// Music

		//AudioSpaceStudio* studio = Audio->AsStudio();

		//studio->SetVolume(0.25f);
		//studio->LoadMasterBank("SpaceGame/Master.bank");
		//studio->LoadMasterBank("SpaceGame/Master.strings.bank");
		//studio->CreateEvent("Music/Attack");

		//auto forestInstance = studio->CreateInstance("Music/Attack", false);
		//Audio->AsStudio()->StartInstance(forestInstance);

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
		Cell debug   = { CellType::DEBUG, CellProperties::DELETE_TIME };
		Cell sand    = { CellType::SAND,  CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };
		Cell water   = { CellType::WATER, CellProperties::MOVE_DOWN | CellProperties::MOVE_SIDE };
		Cell rock    = { CellType::ROCK  };
		Cell metal   = { CellType::METAL };
		Cell debris  = { CellType::DEBRIS };
		Cell smoke   = { CellType::SMOKE,     CellProperties::MOVE_RANDOM | CellProperties::DELETE_TIME };
		Cell explosn = { CellType::EXPLOSION, CellProperties::MOVE_RANDOM | CellProperties::DELETE_TIME };
		
		Cell rez     = { CellType::REZ };

		Cell laser   = { CellType::LASER,   CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_BEAM };
		Cell elaser  = { CellType::eLASER,  CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_BEAM };
		Cell mlaser  = { CellType::mLASER,  CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_BEAM };
		Cell bullet  = { CellType::BULLET,  CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_PROJECTILE };
		Cell missile = { CellType::MISSILE, CellProperties::MOVE_FORWARD | CellProperties::DELETE_TIME | CellProperties::HIT_LIKE_MISSILE };

		Cell bomb    = { CellType::BOMB, CellProperties::MOVE_DOWN | CellProperties::MOVE_DOWN_SIDE };

		empty  .Color = iw::Color::From255(  0,   0,   0, 0);
		debug  .Color = iw::Color::From255(255,   0,   0);
		sand   .Color = iw::Color::From255(237/2, 201/2, 175/2);
		water  .Color = iw::Color::From255(198/2, 233/2, 249/2);
		rock   .Color = iw::Color::From255(201, 201, 201);
		metal  .Color = iw::Color::From255(230, 230, 230);
		debris .Color = iw::Color::From255(150, 150, 150);
		explosn.Color = iw::Color::From255(255,  66,  33);

		rez    .Color = iw::Color::From255(0, 255, 30/*, 200*/);

		smoke  .Color = explosn.Color; // lerped to -> iw::Color::From255(100, 100, 100);
		laser  .Color = iw::Color::From255(255,   0,   0);
		elaser .Color = iw::Color::From255(  0, 200, 255);
		mlaser .Color = iw::Color::From255(255, 128, 20);
		bullet .Color = iw::Color::From255(255, 255,   0);
		missile.Color = metal.Color;

		bomb.Color = iw::Color::From255(50, 230, 50);

		sand .dY = -1;
		water.dY = -1;
		smoke.dX   = smoke.dY = .5f;
		explosn.dX = explosn.dY = 2;

		bullet.dX  = 120; // Initial speeds
		laser.dX   = 1750;
		elaser.dX  = 800;
		mlaser.dX  = 2000;
		missile.dX = 200;
		
		bomb .dY = -1;

		debug.Life = 2*__stepTime;
		laser .Life  = 0.06f;
		elaser.Life  = 1.00f;
		mlaser.Life  = 0.25f;
		bullet.Life  = 0.001f;
		missile.Life = 0.05f;
		explosn.Life = 0.06f;
		smoke.Life   = 0.5f;

		empty  .Precedence = 0;
		debug  .Precedence = 0;
		smoke  .Precedence = 1;
		explosn.Precedence = 100;

		rez    .Precedence = 1000;

		laser  .Precedence = 15;
		elaser .Precedence = 15;
		mlaser .Precedence = 1001;
		bullet .Precedence = 16;
		missile.Precedence = 15;

		bullet.MaxSplitCount = 5;
		laser .MaxSplitCount = 10;
		mlaser.MaxSplitCount = 4;
		elaser.MaxSplitCount = 2;

		laser  .UseFloatingPosition = true;
		elaser .UseFloatingPosition = true;
		mlaser .UseFloatingPosition = true;
		bullet .UseFloatingPosition = true;
		missile.UseFloatingPosition = true;

		Cell::SetDefault(CellType::EMPTY,     empty);
		Cell::SetDefault(CellType::DEBUG,     debug);
		Cell::SetDefault(CellType::SAND,      sand);
		Cell::SetDefault(CellType::WATER,     water);
		Cell::SetDefault(CellType::ROCK,      rock);
		Cell::SetDefault(CellType::METAL,     metal);
		Cell::SetDefault(CellType::DEBRIS,    debris);
		Cell::SetDefault(CellType::SMOKE,     smoke);
		Cell::SetDefault(CellType::EXPLOSION, explosn);
		
		Cell::SetDefault(CellType::REZ,       rez);

		Cell::SetDefault(CellType::LASER,     laser);
		Cell::SetDefault(CellType::eLASER,    elaser);
		Cell::SetDefault(CellType::mLASER,    mlaser);
		Cell::SetDefault(CellType::BULLET,    bullet);
		Cell::SetDefault(CellType::MISSILE,   missile);
		Cell::SetDefault(CellType::BOMB,      bomb);

		// Sand rendering

		m_sandTexture = REF<iw::Texture>( // size of the screen in cells
			Renderer->Width()  / world.m_scale,
			Renderer->Height() / world.m_scale, 
			iw::TEX_2D,
			iw::RGBA
		);
		m_sandTexture->SetFilter(iw::NEAREST);
		m_sandTexture->CreateColors();
		m_sandTexture->Clear();

		iw::ref<iw::Material> sandScreenMat = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		sandScreenMat->SetTexture("texture", m_sandTexture);

		m_sandScreen = Renderer->ScreenQuad().MakeInstance();
		m_sandScreen.SetMaterial(sandScreenMat);

		// minimap

		m_minimapTexture = REF<iw::Texture>( // size of the whole map / 10
			__worldSize*2 / 10,
			__worldSize*2 / 10,
			iw::TEX_2D,
			iw::RGBA
		);
		m_minimapTexture->SetFilter(iw::NEAREST);
		m_minimapTexture->CreateColors();
		m_minimapTexture->Clear();

		iw::ref<iw::Material> minimapMat = REF<iw::Material>(Asset->Load<Shader>("shaders/SpaceGame/minimap.shader"));
		minimapMat->SetTexture("texture", m_minimapTexture);

		m_minimapScreen = Renderer->ScreenQuad().MakeInstance();
		m_minimapScreen.SetMaterial(minimapMat);

		// ammo counts

		iw::ref<iw::Material> teleMat = REF<iw::Material>(Asset->Load<Shader>("shaders/SpaceGame/ammo.shader"));
		teleMat->SetTransparency(iw::Transparency::ADD);

		m_teleScreen = Renderer->ScreenQuad().MakeInstance();
		m_teleScreen.SetMaterial(teleMat);

		// icons

		iw::ref<iw::Texture> energyIconTex  = Asset->Load<iw::Texture>("textures/SpaceGame/energy_icon.png");
		iw::ref<iw::Texture> bulletIconTex  = Asset->Load<iw::Texture>("textures/SpaceGame/bullet_icon.png");
		iw::ref<iw::Texture> missileIconTex = Asset->Load<iw::Texture>("textures/SpaceGame/missile_icon.png");
		energyIconTex ->SetFilter(iw::NEAREST);
		bulletIconTex ->SetFilter(iw::NEAREST);
		missileIconTex->SetFilter(iw::NEAREST);

		iw::ref<iw::Material> energyIconMat  = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		iw::ref<iw::Material> bulletIconMat  = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		iw::ref<iw::Material> missileIconMat = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		energyIconMat ->SetTexture("texture", energyIconTex);
		bulletIconMat ->SetTexture("texture", bulletIconTex);
		missileIconMat->SetTexture("texture", missileIconTex);
		energyIconMat ->SetTransparency(iw::Transparency::ADD);
		bulletIconMat ->SetTransparency(iw::Transparency::ADD);
		missileIconMat->SetTransparency(iw::Transparency::ADD);

		m_energyIconScreen = Renderer->ScreenQuad().MakeInstance();
		m_bulletIconScreen  = Renderer->ScreenQuad().MakeInstance();
		m_missileIconScreen = Renderer->ScreenQuad().MakeInstance();
		m_energyIconScreen .SetMaterial(energyIconMat);
		m_bulletIconScreen .SetMaterial(bulletIconMat);
		m_missileIconScreen.SetMaterial(missileIconMat);

		// Energy

		iw::ref<iw::Material> energyMat = REF<iw::Material>(Asset->Load<Shader>("shaders/simple_color.shader"));
		iw::ref<iw::Material> bulletMat = REF<iw::Material>(Asset->Load<Shader>("shaders/particle/simple_color.shader"));
		iw::ref<iw::Material> missileMat = bulletMat->MakeInstance();
		energyMat ->Set("color", iw::Color::From255(0,   255,   0));
		bulletMat ->Set("color", iw::Color::From255(196, 141,  37));
		missileMat->Set("color", iw::Color::From255(230, 230, 230));
		energyMat ->SetTransparency(iw::Transparency::ADD);
		bulletMat ->SetTransparency(iw::Transparency::ADD);
		missileMat->SetTransparency(iw::Transparency::ADD);
		
		m_energyScreen = Renderer->ScreenQuad().MakeInstance();
		m_energyScreen.SetMaterial(energyMat);

		// bullet & missile particle system

		iw::Mesh bulletMesh  = Renderer->ScreenQuad().MakeCopy();
		iw::Mesh missileMesh = Renderer->ScreenQuad().MakeCopy();
		
		bulletMesh.Data()->TransformMeshData(iw::Transform(0, iw::vector2(2.5f, .1)));
		missileMesh.Data()->TransformMeshData(iw::Transform(0, iw::vector2(.1, 2.5f)));

		bulletMesh.SetMaterial(bulletMat);
		missileMesh.SetMaterial(missileMat);

		m_bullets.SetParticleMesh(bulletMesh);
		m_missiles.SetParticleMesh(missileMesh);

		// Stars

		iw::ref<iw::Shader> starsShader = Asset->Load<Shader>("shaders/particle/simple_color.shader");

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

	int updateIndexS = 0;
	int updateIndexT = 0;

	void FactorySandLayer::PostUpdate() {
		if (reset) {
			Reset();
			reset = false;
		}

		float deltaTime = iw::DeltaTime() > __stepTime ? iw::DeltaTime() : __stepTime;

		int height = m_sandTexture->Height();
		int width  = m_sandTexture->Width();

		vector2 playerLocation = player ? player.Find<iw::Transform>()->Position : 0;

		// camera frustrum
		int fy = iw::clamp<int>(playerLocation.y - height / 2, -__worldSize, __worldSize);
		int fx = iw::clamp<int>(-width  / 2, -__worldSize, __worldSize);
		int fy2 = iw::clamp<int>(fy + height, -__worldSize, __worldSize);
		int fx2 = iw::clamp<int>(fx + width,  -__worldSize, __worldSize);

		vector2 pos = mousePos / world.m_scale;
		pos.x = floor(pos.x) + fx;
		pos.y = floor(height - pos.y) + fy;

		// mouse pos only works with borderless if it opens with the exact resolution :(

		//world.SetCell(pos.x, pos.y, Cell::GetDefault(CellType::DEBUG));

		if (Keyboard::KeyDown(iw::RSHIFT)) {
			reset = true;
		}

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

		iw::ref<iw::Space>& space = Space; // :<, lambdas cant deref pointers, or just ones with names the same to a class?????

		stepTimer -= iw::DeltaTime();
		if (stepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) {
			stepTimer = __stepTime;

			if (reset) return;

			// Sand spawning

			if (Keyboard::KeyDown(iw::O)) {
				world.SetCell(pos.x, pos.y, Cell::GetDefault(CellType::BOMB));
			}

			nextLevelTimer -= deltaTime;
			if (nextLevelTimer < 0) {
				nextLevelTimer = nextLevelTime;

				int type = 0;
				float time = levelTime;

				float r = (iw::randf() + 1) / 2;
				if (r > 0.90) {
					type = 2;
					time = 0;
				}

				else if (r > .5) {
					type = 1;
				}

				else {
					type = 0;
				}

				nextLevels.emplace_back(time, iw::randf() * __worldSizeX, type);
			}

			for (int i = 0; i < nextLevels.size(); i++) {
				auto& [time, location, type] = nextLevels[i];

				switch (type) {
					case 0: world.SetCell(location, __worldSizeY, Cell::GetDefault(CellType::SAND));  break;
					case 1: world.SetCell(location, __worldSizeY, Cell::GetDefault(CellType::WATER)); break;
					case 2: world.SetCell(location, __worldSizeY, Cell::GetDefault(CellType::BOMB)); break;
				}

				time -= deltaTime;
				if (time < 0) {
					nextLevels.erase(nextLevels.begin() + i);
					i--;
				}
			}

			// Player

			Space->Query<iw::Transform, Tile, Player>().Each([&](
				iw::EntityHandle entity,
				iw::Transform* transform,
				Tile* tile,
				Player* player)
			{
				float vx = player->Movement.x * 1;
				float vy = player->Velocity.y - .1f;

				if (player->OnGround) {
					vy += player->Movement.y * 3;
				}

				auto [collision, coords] = ShapeCast(
					transform->Position.x,      transform->Position.y,
					transform->Position.x + vx, transform->Position.y + vy,
					transform, tile
				);

				auto [colX, colY] = collision;

				if (colX || colY) {
					auto [mx, my] = coords;
					if (colX) vx = mx - int(transform->Position.x);
					if (colY) vy = my - int(transform->Position.y);
				}

				player->OnGround = colY;

				player->Velocity = { vx, vy };
				transform->Position += player->Velocity;

				if (   player->Fire
					/*&& player->BulletAmmo > 0 */)
				{
					player->FireTimeout = 0.1;
					Fire(transform, pos, Cell::GetDefault(CellType::BULLET), tile->TileId, true);
				}
			});


							   PasteTiles();
			updatedCellCount = UpdateSandWorld  (fx, fy, fx2, fy2, deltaTime);
			                   UpdateSandTexture(fx, fy, fx2, fy2);
							   RemoveTiles();
		}

		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 5000;

		// Render

		Renderer->BeginScene();

			//// Debug

			//Renderer->BeforeDraw([&, playerLocation, deltaTime]() {
			//	std::stringstream sb;
			//	sb << nextLevelTimer;
			//	//sb << updatedCellCount << "\n" << deltaTime << "\n" << playerLocation << "\n" << mousePos;
			//	m_font->UpdateMesh(m_textMesh, sb.str(), 0.001, 1);
			//});
			//Renderer->DrawMesh(iw::Transform(-.4f), m_textMesh);

			//// Main render

			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);

			Renderer->DrawMesh(iw::Transform(0, 1, iw::quaternion::from_euler_angles(iw::Pi, 0, 0)), testMesh);

		Renderer->EndScene();
	}

	bool FactorySandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };
		return true;
	}

	bool FactorySandLayer::On(
		MouseButtonEvent& e)
	{
		if (Player* p = player.Find<Player>()) {
			switch (e.Button) {
				case iw::val(LMOUSE): p->Fire = e.State; break;
			}
		}

		return true;
	}

	bool FactorySandLayer::On(
		EntityDestroyedEvent& e)
	{
		if (SharedCellData* share = Space->FindComponent<SharedCellData>(e.Entity)) {
			for (Cell* cell : share->m_users) {
				cell->Share = nullptr;
			}
		}

		return true;
	}

	bool FactorySandLayer::On(
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
			case iw::val(SPACE): {
				p->Movement.y = e.State ? 1 : 0; break;
			}
		}

		return true;
	}

	void FactorySandLayer::Reset() {
		world.Reset();
		Space->Clear();
		m_minimapTexture->Clear();

		srand(iw::Ticks());

		GenerateWorld();

		//tile player

		player = Space->CreateEntity<iw::Transform, Player, Tile>();
		player.Set<iw::Transform>();
		Tile* tile = player.Set<Tile>(std::vector<iw::vector2> {
			vector2(0, 0),
			vector2(1, 0),
			vector2(2, 0),
			vector2(2, 1),
			vector2(2, 2),
		}, 3);
		player.Set<Player>();

		// testing

		// Here's the process

		// Game logic update

		// Move cells from inside of AABB with the same tileid into a field
		// Scan field for polygon, if something is inside of the ABBB, also make triangles and do collision detection
		// Run physics
		// Run sand
		// Move cells from field back into the world, on collision eject cells
		
		// Render cells

		iw::AABB2 bounds = tile->AABB();

		//auto [sizeX, sizeY] = bounds.Max - bounds.Min;

		// turn tile into field

		unsigned field[17*17] = {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};

		std::vector<iw::vector2> polygon = iw::common::MakePolygonFromField(field, 17, 17, 1u);
		std::vector<unsigned>    index   = iw::common::TriangulatePolygon(polygon);

		float maxLength = 0;
		for (iw::vector2& v : polygon) {
			if (v.length_squared() > maxLength) {
				maxLength = v.length_squared();
			}
		}

		maxLength = sqrt(maxLength);

		for (iw::vector2& v : polygon) {
			v /= maxLength;
		}

		iw::MeshDescription description;
		description.DescribeBuffer(bName::POSITION, MakeLayout<float>(2));

		iw::MeshData* data = new iw::MeshData(description);

		data->SetBufferData(bName::POSITION, polygon.size(), polygon.data());
		data->SetIndexData(index.size(), index.data());

		iw::ref<iw::Texture> texture = REF<iw::Texture>(17, 17);
		texture->CreateColors();
		texture->SetFilter(iw::NEAREST);
		for (int i = 0; i < 17 * 17; i++) {
			((unsigned*)texture->Colors())[i] = field[i] == 1 ? -1 : 0;
		}

		iw::ref<iw::Material> material = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/texture2D.shader"));
		material->SetTexture("texture", texture);

		testMesh = data->MakeInstance();
		testMesh.SetMaterial(material);





		// shared cell player
		//player = Space->CreateEntity<Player, SharedCellData>();
		//                    player.Set<Player>();
		//SharedCellData* s = player.Set<SharedCellData>();

		//s->UsedForMotion = true;

		//Cell c = Cell::GetDefault(CellType::METAL);
		//c.Props = CellProperties::MOVE_SHARED_USER;
		//c.UseFloatingPosition = true;
		//c.Share = s;

		//world.SetCell(0, 0, c);
		//world.SetCell(1, 0, c);
		//world.SetCell(0, 1, c);
		//world.SetCell(1, 1, c);
	}

	void FactorySandLayer::GenerateWorld() {

	}

	int FactorySandLayer::UpdateSandWorld(
		int fx,  int fy,
		int fx2, int fy2,
		float deltaTime)
	{
		world.Step();

		std::mutex chunkCountMutex;
		std::condition_variable chunkCountCV;
		int chunkCount = 0;
		int cellsUpdatedCount = 0;

		//int minX = fx  - world.m_chunkWidth * 10; // update only around player, cant really do this though
		//int maxX = fx2 + world.m_chunkWidth * 10;

		//int minY = fy  - world.m_chunkHeight * 10;
		//int maxY = fy2 + world.m_chunkHeight * 10;
												  // excessive updating
		auto [minCX, minCY] = iw::vector2(-__arenaSize, -__arenaSize);   // world.GetChunkLocation(minX, minY);
		auto [maxCX, maxCY] = iw::vector2( __arenaSize,  __arenaSize)+1; // world.GetChunkLocation(maxX, maxY);

		std::vector<SandChunk*> chunksUpdated;

		for (int px = 0; px < 2; px++)
		for (int py = 0; py < 2; py++) {
			for (int x = minCX + px; x < maxCX; x += 2)
			for (int y = minCY + py; y < maxCY; y += 2) {
				SandChunk* chunk = world.GetChunkDirect(x, y);

				if (!chunk) continue;

				chunksUpdated.push_back(chunk);

				if (chunk->IsAllEmpty() || chunk->IsAllNoProps()) continue;

				{ std::unique_lock lock(chunkCountMutex); chunkCount++; }
				cellsUpdatedCount += chunk->m_width*chunk->m_height; // would be area of rect

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
		
		//chunkCount = chunksUpdated.size();

		for (SandChunk* chunk : chunksUpdated) {
			//Task->queue([&, chunk]() {
				chunk->CommitMovedCells(world.m_currentTick);

				//{  std::unique_lock lock(chunkCountMutex); chunkCount--; }
				//chunkCountCV.notify_one();
			//});
		}

		//{
		//	std::unique_lock lock(chunkCountMutex);
		//	chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		//}

		return cellsUpdatedCount;
	}

	void FactorySandLayer::UpdateSandTexture(
		int fx,  int fy,
		int fx2, int fy2)
	{
		std::mutex chunkCountMutex;
		std::condition_variable chunkCountCV;
		int chunkCount = 0;

		iw::ref<iw::Texture> sandTex = m_sandTexture;    //m_sandScreen.Material()->GetTexture("texture");
		iw::ref<iw::Texture> miniTex = m_minimapTexture;
			 
		sandTex->Clear(); // minitexture gets cleared in Reset
		unsigned int* sandColors = (unsigned int*)sandTex->Colors(); // cast from r, g, b, a to rgba
		unsigned int* miniColors = (unsigned int*)miniTex->Colors();

		bool showChunks = Keyboard::KeyDown(K);

		for (SandChunk* chunk : world.GetVisibleChunks(fx, fy, fx2, fy2)) {
			{ std::unique_lock lock(chunkCountMutex); chunkCount++; }

			Task->queue([=, &chunkCount, &chunkCountCV, &chunkCountMutex]() {
				int startY = iw::clamp(fy  - chunk->m_y, 0, chunk->m_height);
				int startX = iw::clamp(fx  - chunk->m_x, 0, chunk->m_width);
				int endY   = iw::clamp(fy2 - chunk->m_y, 0, chunk->m_height);
				int endX   = iw::clamp(fx2 - chunk->m_x, 0, chunk->m_width);

				// sand texture
				for (int y = startY; y < endY; y++)
				for (int x = startX; x < endX; x++) {
					int texi = (chunk->m_x + x - fx) + (chunk->m_y + y - fy) * sandTex->Width();

					if (showChunks) {
						const Cell& cell = chunk->GetCellDirect(x, y);

						if (cell.Type != CellType::EMPTY) {
							sandColors[texi] = cell.Color;
						}

						if (x == 0 || y == 0) {
							sandColors[texi] = iw::Color(chunk->IsAllEmpty() ? 0 : 1, 0, chunk->IsAllEmpty() ? 1 : 0, 1);
						}

						if (   x == chunk->m_minX || y == chunk->m_minY
							|| x == chunk->m_maxX || y == chunk->m_maxY)
						{
							sandColors[texi] = iw::Color(0, 1, 0, 1);
						}
					}

					else {
						sandColors[texi] = chunk->GetCellDirect(x, y).Color;
					}
				}

				// minimap
				for (int y = startY; y < endY; y += 10)
				for (int x = startX; x < endX; x += 10) {
					int texi = (chunk->m_x + x + __worldSize)/10 + (chunk->m_y + y + __worldSize)/10 * miniTex->Width();

					if (texi > miniTex->Width() * miniTex->Height()) {
						LOG_INFO << "asd";
					}

					// Theme the minimap based on tileid, enemys should be red everything else green or something idk
					miniColors[texi] = chunk->GetCellDirect(x, y).Color;
				}

				{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
				chunkCountCV.notify_one();
			});
		}

		{
			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

		sandTex->Update(Renderer->Device); 
		miniTex->Update(Renderer->Device); // should be auto in renderer 
	}

	bool FactorySandLayer::PasteTile(iw::Transform* transform, Tile* tile, bool test) {
		float angle = transform->Rotation.euler_angles().z;

		SandChunk* chunk = nullptr;

		bool anyCollision = false;

		for (int i = 0; i < tile->Locations.size(); i++) {
			auto [x, y] = TranslatePoint(tile->Locations[i], transform->Position, angle);

			if (!chunk || !chunk->InBounds(x, y)) chunk = world.GetChunk(x, y); // chunk cache, lil speed up
			if (!chunk) {
				anyCollision = true;
				if (test) {
					break;
				}

				continue;
			}

			const Cell& cell = chunk->GetCell(x, y);

			if (   cell.Type == CellType::EMPTY
				|| cell.Type == CellType::DEBUG
				|| cell.TileId == tile->TileId
				|| cell.Precedence < tile->Precedence)
			{
				if (!test) {
					Cell me = Cell::GetDefault(CellType::METAL);
					me.TileId = tile->TileId;
					me.Life = 1;

					chunk->SetCell(x, y, me, world.m_currentTick);
				}
			}

			else {
				anyCollision = true;
				if (!test) {
					//tile->Locations[i] = tile->Locations.back(); tile->Locations.pop_back();
					//i--;
				}

				else {
					break;
				}
			}
		}

		return anyCollision;
	}

	void FactorySandLayer::PasteTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			PasteTile(t, a);
		});
	}
	
	void FactorySandLayer::RemoveTiles() {
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
				
				if (!chunk) continue;

				const Cell& cell = chunk->GetCell(x, y);

				if (cell.Type == CellType::EMPTY) continue;

				if (   cell.TileId == a->TileId
					|| cell.Precedence < a->Precedence)
				{
					chunk->SetCell(x, y, Cell::GetDefault(CellType::EMPTY), world.m_currentTick);
				}

				else {
					//a->Locations[i] = a->Locations.back(); a->Locations.pop_back();
					//i--;
				}
			}
		});
	}
}
