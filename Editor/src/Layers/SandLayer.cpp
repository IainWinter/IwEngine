#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"


double __chunkScale = 1.5f;
int __chunkSize = 150;
int __arenaSize = 50;
int __arenaSizeBuf = 1;
int __brushSizeX = 50;
int __brushSizeY = 50;
float __stepTime = 1/500.f;
bool __stepDebug = false;

int __worldSize = __arenaSize * __chunkSize / __chunkScale; // gets set below

int __worldSizeX = 8;
int __worldSizeY = 50;

int __seaHeight = 125;

//#define _DEBUG 1

namespace iw {
	SandLayer::SandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(__chunkSize, __chunkSize, __chunkScale))
		, reset(true)
	{
		world.SetMaxChunks(__worldSizeX, __worldSizeY);

		__worldSizeX *= __chunkSize / __chunkScale;
		__worldSizeY *= __chunkSize / __chunkScale;
	}

	int SandLayer::Initialize() {
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

		empty  .Color = iw::Color::From255(  0,   0,   0, 0);
		debug  .Color = iw::Color::From255(255,   0,   0);
		sand   .Color = iw::Color::From255(237, 201, 175);
		water  .Color = iw::Color::From255(175, 201, 237);
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

		sand .dY = -1;
		water.dY = -1;
		smoke.dX   = smoke.dY = .5f;
		explosn.dX = explosn.dY = 2;

		sand .Gravitised = true;
		water.Gravitised = true;

		bullet.dX  = 1200; // Initial speeds
		laser.dX   = 1750;
		elaser.dX  = 800;
		mlaser.dX  = 2000;
		missile.dX = 200;

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
		int fy = iw::clamp<int>(playerLocation.y - height / 10, -__worldSize, __worldSize);
		int fx = iw::clamp<int>(playerLocation.x - width  / 2, -__worldSize, __worldSize);
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
			
			nextLevelTimer -= deltaTime;
			if (nextLevelTimer < 0) {

				if (nextPlayerLocation == -1) {
					nextPlayerLocation = player.Find<iw::Transform>()->Position.y + 700;

					GenerateWorld(nextPlayerLocation);
				}

				player.Find<Physical>()->Velocity.y = 150;

				if (player.Find<iw::Transform>()->Position.y > nextPlayerLocation) {
					player.Find<iw::Transform>()->Position.y = nextPlayerLocation;
					player.Find<Physical>()->Velocity.y = 0;

					nextPlayerLocation = -1;
					nextLevelTimer = nextLevelTime;
				}
			}

			// Update one radius per frame
			{
			int index = 0;
			bool needUpdate = true;
			
			Space->Query<SharedCellData, Physical>().Each([&, space](auto e, auto s, auto p) {
				//LOG_INFO << "Share update " << updateIndexS;
				
				if (   needUpdate
					&& updateIndexS == index)
				{
					updateIndexS = index + 1;
					needUpdate = false;

					p->Radius = s->Radius() * (p->Solid ? 2 : 1);
				}

				index++;
			});
			if (updateIndexS >= index) updateIndexS = 0; // reset updateIndex
			
			index = 0;
			needUpdate = true;
			
			Space->Query<Tile, Physical>().Each([&](auto, auto a, auto p) {
				//LOG_INFO << "Tile update " << updateIndexT;

				if (   needUpdate
					&& updateIndexT == index)
				{
					updateIndexT = index + 1;
					needUpdate = false;

					p->Radius = 100 + a->Radius() * 5;
				}

				index += 1;
			});
			if (updateIndexT >= index) updateIndexT = 0; // reset updateIndex
			}
			
			// Flocking

			Space->Query<iw::Transform, Flocking, Physical>().Each([&, space](auto e, auto t, auto f, auto p) {
				if (!f->Active) return; // for missiles

				iw::vector2 avgAway = 0;
				iw::vector2 avgCenter = 0;
				iw::vector2 avgForward = 0;
				int count = 0;

				space->Query<iw::Transform, Physical>().Each([&](auto e2, auto t2, auto p2) {
					if (e == e2) return;

					if (   p->HasTarget
						&& p->AttractTarget
						&& (p->Target - t2->Position).length_squared() < 20*20)
					{
						return;
					}

					// deviates when mining but not much

					iw::vector2 away = t->Position - t2->Position;
					if (away.length_squared() > p2->Radius*p2->Radius) { // exit if too far or current target (above)
						return;
					}

					away *= p2->Radius / iw::clamp(away.length() - p2->Radius/2, 1.f, p2->Radius);

#ifdef _DEBUG
					for (auto [x, y] : FillCircle(t2->Position.x, t2->Position.y, p2->Radius)) {
						if (world.IsEmpty(x, y)) world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
					}
					for (auto [x, y] : FillLine(t->Position.x, t->Position.y, t->Position.x + away.x, t->Position.y + away.y)) {
						Cell c = Cell::GetDefault(CellType::DEBUG);
						c.Color = iw::Color(0, 1, 0);
						if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
					}
#endif

					avgAway    += away;
					avgCenter  += t2->Position;
					avgForward += p2->Velocity;
					count      += 1;
				});

 				if (count > 0) {
					avgAway    /= count;
					avgCenter  /= count; avgCenter -= t->Position;
					avgForward /= count;
				}

				avgCenter  = 0; // only using away force for now
				avgForward = 0;

				if(p->HasTarget) {
#ifdef _DEBUG
					for (auto [x, y] : FillCircle(p->Target.x, p->Target.y, p->TargetRadius)) {
						Cell c = Cell::GetDefault(CellType::DEBUG);
						c.Color = iw::Color(1, 1, 0);
						if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
					}
#endif
					iw::vector2 at = t->Position - p->Target;

					if (at.length_squared() < p->TargetRadius*p->TargetRadius*p->Velocity.length_squared()) {
						avgAway += at * (p->AttractTarget ? -1 : 1/(at.length() - p->TargetRadius));
					}
				}

#ifdef _DEBUG
				for (auto [x, y] : FillLine(t->Position.x, t->Position.y, t->Position.x + (avgAway + avgForward+ avgCenter).x, t->Position.y + (avgAway + avgForward + avgCenter).y)) {
					Cell c = Cell::GetDefault(CellType::DEBUG);
					c.Color = iw::Color(0, 1, 0);
					if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
				}
#endif
				// Flocking force, should cap acceleration

				iw::vector2 dd = avgAway + avgForward + avgCenter;
				dd = iw::clamp<iw::vector2>(dd, -100, 100);

				p->Velocity = iw::lerp(p->Velocity, p->Velocity + dd, deltaTime);

#ifdef _DEBUG
				for (auto [x, y] : FillLine(t->Position.x, t->Position.y, t->Position.x + p->Velocity.x, t->Position.y + p->Velocity.y)) {
					Cell c = Cell::GetDefault(CellType::DEBUG);
					c.Color = iw::Color(0, 0, 1);
					if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
				}
#endif
				// Target force, make own funciton in iwmath, i guess a turn twoards
				iw::vector2 nVel = (p->Velocity).normalized();
				iw::vector2 nDir = (p->Target - t->Position).normalized();
				iw::vector2 delta = (nDir - nVel) * p->Speed * p->TurnRadius;

				p->Velocity = (p->Velocity + delta).normalized() * p->Speed;
			});

			// Enemy

			bool targeting = false; 
			float minDist = FLT_MAX;
			iw::vector2 targetPos, targetVel;
			int supplyShipCout = 0;

			{
			Space->Query<iw::Transform, EnemyShip, Physical>().Each([&](auto e, auto t, auto s, auto p) {

				// set players targeting

				float distToCursor = (t->Position - pos).length_squared();
				if (distToCursor < 250*250 && distToCursor < minDist) {
					minDist = distToCursor;
					targetPos = t->Position;
					targetVel = p->Velocity;
					targeting = true;
				}

				// Check for another command every few seconds

				s->RezLow      = s->Rez <= s->MinRez;
				s->AtObjective = (t->Position - s->Objective).length_squared() < 200*200;  // 200 meters form objective, objectives should have a radius
				s->AttackMode  = s->PlayerVisible;

				p->AttractTarget = s->Homecoming;

				// Always move twoards objective

				if (s->AttackMode) {
					p->Speed = iw::lerp(p->Speed, s->Speed*2, deltaTime); // fast when attacking
				}

				else if (s->AtObjective) {
					if (space->HasComponent<EnemySupplyShip>(e)) {
						p->Speed = iw::lerp(p->Speed, 10.f, deltaTime*7); // slow at objective
					}
					else {
						p->Speed = iw::lerp(p->Speed, s->Speed/5, deltaTime); // slow at objective
					}
				}

				else {
					p->Speed = iw::lerp(p->Speed, s->Speed, deltaTime); // normal
				}
				
				p->Target = s->Objective;
				p->HasTarget = true;
			});

			Space->Query<iw::Transform, EnemyShip, EnemyAttackShip>().Each([&](auto, auto t, auto s, auto) {
				s->AttackMode |= (t->Position - playerLocation).length_squared() < 500*500;
				if (s->AttackMode && (playerLocation - t->Position).length_squared() < 200*200) {
					s->FireTimer -= deltaTime;
					if (s->FireTimer <= 0) {
						s->FireTimer = s->FireTime * (iw::randf() + 1.1f);
						s->Rez -= s->RezToFireLaser;

						Fire(t, playerLocation, 0, Cell::GetDefault(CellType::eLASER), true); // should shoot forward, not just at player location
					}

					s->Objective = playerLocation;
				}
			});

			Space->Query<iw::Transform, EnemyShip, EnemySupplyShip>().Each([&, space](auto e, auto t, auto s, auto ss) {
				supplyShipCout++;

				if (s->AtObjective) {
					Fire(t, s->Objective , 0, Cell::GetDefault(CellType::mLASER), -1);
				}

#ifdef _DEBUG
				for (auto [x, y] : FillCircle(s->Objective.x, s->Objective.y, 100)) {
					if (world.IsEmpty(x, y)) world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
				}
#endif
			});

			Space->Query<SharedCellData, Missile, Physical, Flocking>().Each([&, space](auto e, auto s, auto m, auto p, auto f) {
				m->Timer += deltaTime;
				if (   m->Timer > m->WaitTime
					&& m->Timer < m->WaitTime + m->BurnTime)
				{
					f->Active = true;

					iw::vector2 mpos(s->pX, s->pY);
					iw::vector2 mvel(s->vX, s->vY);
				
					iw::vector2 closest;
					float minDist = FLT_MAX;
					space->Query<iw::Transform, EnemyShip>().Each([&](auto, auto t2, auto) {
						iw::vector2 v = t2->Position - mpos;
						if (v.length_squared() < minDist) {
							minDist = v.length_squared();
							closest = t2->Position;
						}
					});

					p->HasTarget = false;

					if (minDist != FLT_MAX) {
						if (minDist < 800) {
							s->Hit = true; // explode if near enemy
							return;
						}
					
						p->Target = closest;
						p->HasTarget = true;
					}

					p->Speed = iw::clamp<float>(p->Speed + 300 * deltaTime,
						Cell::GetDefault(CellType::MISSILE).Speed(),
						Cell::GetDefault(CellType::MISSILE).Speed()*5
					);

					s->Life = iw::clamp<float>(s->Life - 3 * deltaTime,
						Cell::GetDefault(CellType::MISSILE).Life / 5,
						Cell::GetDefault(CellType::MISSILE).Life
					);

					// Spawn smoke

					iw::vector2 spos = mpos - mvel * deltaTime * 2;

					Cell smoke = Cell::GetDefault(CellType::SMOKE);
					smoke.TileId = m->TileId;
					smoke.pX = spos.x;
					smoke.pY = spos.y;
					smoke.Life = 10 + iw::randf() * 3;

					world.SetCell(smoke.pX, smoke.pY, smoke);
				}

				else if (m->Timer > m->WaitTime + m->BurnTime) {
					f->Active = false;
					p->TurnRadius = 0; // dont send back into level

					//space->QueueEntity(e, [](Entity entity) { // cont do this until there is an entity changed event
					//	entity.RemoveComponent<Flocking>(); // remove flocking after fuel has run out
					//});
				}

				if (s->Hit) { // if inside of another missiles blast, change velocity a lil
					iw::vector2 v(s->pX - s->hX, s->pY - s->hY);
					v.normalize();

					p->Velocity.x += v.x * (iw::randf() + 1)/5;
					p->Velocity.y += v.y * (iw::randf() + 1)/5;
					s->Hit = false;
				}
			});
			}
			
			// Player

			Space->Query<iw::Transform, Player, Physical>().Each([&](auto e, auto t, auto p, auto pp) {			
				float speed = 250;

				pp->Velocity = iw::lerp(pp->Velocity, iw::vector2(p->Movement.x * speed, 0), deltaTime);

				// Bullets n such

				iw::vector2 target = pos;

				p->FireTimeout -= deltaTime;
				if (   p->FireTimeout <  0
					&& p->FireButtons != 0)
				{
					if (   p->FireButtons.x == 1
						&& p->BulletAmmo > 0)
					{
						p->FireTimeout = 0.025;
						//p->BulletAmmo--;
						
						Fire(t, target +iw::vector2(iw::randf(), iw::randf())*5, pp->Velocity, Cell::GetDefault(CellType::BULLET), true);
					}
					
					else if (p->FireButtons.y == 1
						  && p->MissileAmmo > 0)
					{
						p->FireTimeout = 0.01f;
						//p->MissileAmmo--;

						vector2 d = t->Position + iw::vector2(0, 1);
						FireMissile(t, d, pos, Cell::GetDefault(CellType::MISSILE));
					}
				}

				// recharge bullets

				else if (p->FireTimeout <= -0.25f) {
					if (   p->FireTimeout < -0.5f
						&& p->FireButtons.x == 0
						&& p->BulletAmmo < p->MaxBulletAmmo)
					{
						p->BulletAmmo += 1;
						if (p->BulletAmmo % 5 == 0) {
							p->FireTimeout = -0.25f;
						}
					}

					if (   p->FireButtons.y == 0 // no mining
						&& p->Movement.z    == 0 // no boost
						&& p->Energy < p->MaxEnergy) // has no delay, should prob have another timer for each recharge
					{
						p->Energy += deltaTime * 10;
					}

					//if (   p->FireButtons.z == 0
					//	&& p->MissileAmmo < p->MaxMissileAmmo)
					//{
					//	p->MissileAmmo += 1;
					//	if (p->MissileAmmo % 2 == 0) { // get by mining rez not regen, this breaks timer btw
					//		p->FireTimeout = 0;
					//	}
					//}
				}
			});

			// Turn around objects getting close to outside arena

			Space->Query<iw::Transform, Physical>().Each([&](auto e, auto t, auto p) {
				if (t->Position.length_squared() >= (__arenaSize-__arenaSizeBuf)*(__arenaSize-__arenaSizeBuf) * __chunkSize*__chunkSize) {
					p->Velocity = iw::lerp<iw::vector2>(p->Velocity, -t->Position, deltaTime/5);

					// Target force
					iw::vector2 nVel = (p->Velocity).normalized();
					iw::vector2 nDir = (p->Target - t->Position).normalized();
					iw::vector2 delta = (nDir - nVel) * p->Speed * p->TurnRadius;

					p->Velocity = (p->Velocity + delta).normalized() * p->Speed;
					p->Target = -t->Position.normalized() * 100;
				}
			});

			// Move by velocity

			Space->Query<iw::Transform,  Physical>().Each([&, space](auto, auto t, auto p) {
				t->Position += p->Velocity * deltaTime;
			});
			Space->Query<SharedCellData, Physical>().Each([&, space](auto, auto s, auto p) {
				if (!s->UsedForMotion) return;
				if (s->AssignMotion) {
					s->vX = p->Velocity.x;
					s->vY = p->Velocity.y;
				}
			});
			Space->Query<SharedCellData>().Each([&, space](auto e, auto s) {
				s->HitCells.clear();

				if (!s->UsedForMotion) return;
				if (s->AssignMotion) {
					iw::vector2 p = space->FindComponent<iw::Transform>(e)->Position;
					s->pX = p.x;
					s->pY = p.y;
				}

				else {
					s->pX += s->vX * deltaTime;
					s->pY += s->vY * deltaTime;
				}
			});

			// Desotry entities with tile/shareddata if they have no more cells

			auto q = Space->MakeQuery<SharedCellData>(); q->SetNone({ Space->GetComponent<Tile>() });
			for (auto itr : Space->Query(q)) {
				SharedCellData* s = itr.Components.Get<SharedCellData, 0>();
				if (s->UserCount == 0) {
					space->QueueEntity(itr.Handle, iw::func_Destroy);
				}
			}
			Space->Query<iw::Transform, Tile>().Each([&, space](auto e, auto t, auto a) {
				if (a->Locations.size() < a->InitialLocationsSize / 3) {
					if (space->HasComponent<Player>(e)) {
						reset = true; // player is ded
					}

					PasteTile(t, a, true, deltaTime);

					(*Space).QueueEntity(e, iw::func_Destroy);
					return;
				}
			});

			if (reset) return;

			// Need to execute the queue here to clean up shared cells

			Space->ExecuteQueue();

			// Sand update

			// Paste tiles up where to keep tile cells after they die

							   PasteTiles();
			// Line of sight checks need to see Tiles
			
			Space->Query<iw::Transform, EnemyShip, Tile>().Each([&](auto, auto t, auto s, auto a) {
				iw::vector2 d = playerLocation - t->Position;
				d.normalize();
				d *= 1000; // max sightline

				auto cells = RayCast(t->Position.x, t->Position.y, t->Position.x + d.x, t->Position.y + d.y);

				for (const Cell* c : cells) {
					if (c->TileId == a->TileId) continue;
					
					if (c->TileId == 0) {
						s->PlayerVisible = false;
						break;
					}

					if (c->TileId == player.Find<Tile>()->TileId) {
						s->PlayerVisible = true;
						break;
					}
				}
			});

			updatedCellCount = UpdateSandWorld  (fx, fy, fx2, fy2, deltaTime);
			                   UpdateSandTexture(fx, fy, fx2, fy2);
			                   RemoveTiles();

			// Reset hit for shared cell

			//for (auto itr : Space->Query<SharedCellData>()) {
			//	SharedCellData* s = itr.Components.Get<SharedCellData, 0>();

			//	s->Hit = false;
			//	s->hX = 0;
			//	s->hY = 0;
			//}
		}

		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 5000;
		m_minimapScreen.Material()->Set("textureOffset", (playerLocation + __worldSize) / (__worldSize * 2));

		int playerBulletAmmo  = player ? player.Find<Player>()->BulletAmmo : 0;
		int playerMissileAmmo = player ? player.Find<Player>()->MissileAmmo : 0;
		float energy          = player ? player.Find<Player>()->Energy : 0;
		float maxEnergy       = player ? player.Find<Player>()->MaxEnergy : 0; // need cus its a %

		// Render

		while (playerBulletAmmo  != m_bullets .ParticleCount()) {
			if (playerBulletAmmo > m_bullets.ParticleCount()) {
				
				int x = m_bullets.ParticleCount() / 5;

				m_bullets.SpawnParticle(iw::Transform(
					iw::vector2(
						x * 6,
						m_bullets.ParticleCount() - x * 5
					) + iw::vector2(2.5, .1)
				));
			}

			else {
				m_bullets.DeleteParticle(m_bullets.ParticleCount() - 1);
			}
		}
		while (playerMissileAmmo != m_missiles.ParticleCount()) {
			if (playerMissileAmmo > m_missiles.ParticleCount()) {
				
				int x = m_missiles.ParticleCount() / 2;

				m_missiles.SpawnParticle(iw::Transform(
					iw::vector2(
						m_missiles.ParticleCount() * 1 + x * 0.5,
						0
					) + iw::vector2(.1, 2.5)
				));
			}

			else {
				m_missiles.DeleteParticle(m_missiles.ParticleCount() - 1);
			}
		}

		m_bullets .UpdateParticleMesh();
		m_missiles.UpdateParticleMesh();

		Renderer->BeginScene();

			//// UI

			//// can scale by pixels or % of screen
			//iw::vector2 pixels(1.0f / Renderer->Width(), 1.0f / Renderer->Height());
			//iw::vector2 aspect(1.0f, float(Renderer->Width()) / Renderer->Height());
			//																				 //iw::vector2 aspectY(float(Renderer->Height()) / Renderer->Width(), 1); // could be useful
			//
			//// minimap

			//iw::vector2 mapPos(1, -1);
			//iw::vector2 mapScale = .15 * aspect;

			//if (Keyboard::KeyDown(iw::M)) {
			//	mapScale *= 2;
			//	m_minimapScreen.Material()->Set("textureScale", 3.f);
			//}
			//else {
			//	m_minimapScreen.Material()->Set("textureScale", 8.f);
			//}

			//Renderer->DrawMesh(iw::Transform(iw::vector3(mapPos + iw::vector2(-mapScale.x, mapScale.y), -1), mapScale), m_minimapScreen);

			//float hide = 0;

			//if (Keyboard::KeyDown(iw::H)) {
			//	hide = 300;
			//}

			//// tele

			//float teleZ = -.05;

			//iw::vector3 telePos = iw::vector2(-1, -1) - iw::vector2(100, 900 + hide)*pixels;
			//iw::vector3 teleScale = 600 * pixels;

			//Renderer->DrawMesh(iw::Transform(iw::vector3(telePos + teleScale, teleZ), teleScale), m_teleScreen);

			//// energy

			//float iconZ = -0.1f;

			//iw::vector2 dispScale  =     16*pixels;
			//iw::vector2 iconScale  =     48*pixels;
			//iw::vector2 iconPos    = -1 + 8*pixels;
			//float       iconMargin =      8*pixels.y;

			//iw::vector2 eiScale = iconScale/2;

			//iw::vector2 eiPos = iconPos + iw::vector2(iconScale.x, -hide);
			//iw::vector2 biPos = iconPos + iw::vector2(0, 2*  eiScale.y + iconMargin - hide);
			//iw::vector2 miPos = biPos   + iw::vector2(0, 2*iconScale.y + iconMargin - hide);

			//Renderer->DrawMesh(iw::Transform(iw::vector3(eiPos + eiScale,   iconZ), eiScale),   m_energyIconScreen);
			//Renderer->DrawMesh(iw::Transform(iw::vector3(biPos + iconScale, iconZ), iconScale), m_bulletIconScreen);
			//Renderer->DrawMesh(iw::Transform(iw::vector3(miPos + iconScale, iconZ), iconScale), m_missileIconScreen);

			//iw::vector2 ePos = eiPos + iw::vector2(eiScale.x*2 + iconMargin, eiScale.y/2 - hide);
			//iw::vector2 bPos = biPos + iw::vector2(iconScale.x * 2, 0 - hide);
			//iw::vector2 mPos = miPos + iw::vector2(iconScale.x * 2, 0 - hide);

			//iw::vector2 eScale = dispScale * iw::vector2(energy/maxEnergy * 25, 1);

			//Renderer->DrawMesh(iw::Transform(iw::vector3(ePos + eScale,    iconZ), eScale),    m_energyScreen);
			//Renderer->DrawMesh(iw::Transform(iw::vector3(bPos + dispScale, iconZ), dispScale), m_bullets.GetParticleMesh());
			//Renderer->DrawMesh(iw::Transform(iw::vector3(mPos + dispScale, iconZ), dispScale), m_missiles.GetParticleMesh());

			//// Debug

			Renderer->BeforeDraw([&, playerLocation, deltaTime]() {
				std::stringstream sb;
				sb << nextLevelTimer;
				//sb << updatedCellCount << "\n" << deltaTime << "\n" << playerLocation << "\n" << mousePos;
				m_font->UpdateMesh(m_textMesh, sb.str(), 0.001, 1);
			});
			Renderer->DrawMesh(iw::Transform(-.4f), m_textMesh);

			// Main render

			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);
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
		EntityDestroyedEvent& e)
	{
		if (SharedCellData* share = Space->FindComponent<SharedCellData>(e.Entity)) {
			for (Cell* cell : share->m_users) {
				cell->Share = nullptr;
			}
		}

		return false;
	}

	bool SandLayer::On(
		MouseWheelEvent& e)
	{
		if (!player) return false;
		Player* p = player.Find<Player>();
		p->FireButtons.z = e.Delta == 0 ? 0 : 1;
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
				p->Movement.z = e.State ? 1 : 0; break; // (int)
			}
		}

		return false;
	}

	int SandLayer::UpdateSandWorld(
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
		/*		int cellsUpdatedCount = 0;

		world.Step();

		int minX = fx  - world.m_chunkWidth * 10; // update only around player, cant really do this though
		int maxX = fx2 + world.m_chunkWidth * 10;

		int minY = fy  - world.m_chunkHeight * 10;
		int maxY = fy2 + world.m_chunkHeight * 10;
												  // excessive updating
		auto [minCX, minCY] = iw::vector2(-__arenaSize, -__arenaSize);   // world.GetChunkLocation(minX, minY);
		auto [maxCX, maxCY] = iw::vector2( __arenaSize,  __arenaSize)+1; // world.GetChunkLocation(maxX, maxY);

		std::vector<SandChunk*> chunksUpdated;

		for (int px = 0; px < 2; px++)
		for (int py = 0; py < 2; py++) {
			std::vector<SandChunk*> chunksToUpdate;

			for (int x = minCX + px; x < maxCX; x += 2)
			for (int y = minCY + py; y < maxCY; y += 2) {
				SandChunk* chunk = world.GetChunkDirect(x, y);

				if (!chunk) continue;
				chunksUpdated.push_back(chunk);    // if not null, better to find which chunks got touched

				if (chunk->IsAllEmpty() || chunk->IsAllNoProps()) continue;
				chunksToUpdate.push_back(chunk);  // if has any cells that need an update

				cellsUpdatedCount += chunk->m_width * chunk->m_height;
			}

			ExecuteAndWait(chunksToUpdate, [&](SandChunk* chunk) {
				DefaultSandWorker(world, *chunk, Space, deltaTime).UpdateChunk();
			});
		}

		ExecuteAndWait(chunksUpdated, [&](SandChunk* chunk) {
			chunk->CommitMovedCells(world.m_currentTick);
		});

		return cellsUpdatedCount;*/
	}

	void SandLayer::UpdateSandTexture(
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
	
	void SandLayer::PasteTile(iw::Transform* transform, Tile* tile, bool death, float deltaTime) {
		float angle = transform->Rotation.euler_angles().z;

		SandChunk* chunk = nullptr;

		for (int i = 0; i < tile->Locations.size(); i++) {
			auto [x, y] = TranslatePoint(tile->Locations[i], transform->Position, angle);

			if (!chunk || !chunk->InBounds(x, y)) {
				chunk = world.GetChunk(x, y);
			}

			if (!chunk) continue;

			const Cell& cell = chunk->GetCell(x, y);

			Cell me = Cell::GetDefault(CellType::METAL);
			me.TileId = tile->TileId;
			me.Life = 1;

			bool die = death;

			if (   cell.Type != CellType::EMPTY
				&& cell.Type != CellType::DEBUG
				&& cell.TileId != tile->TileId
				&& cell.Precedence > tile->Precedence)
			{
				die = true;
				tile->Locations[i] = tile->Locations.back(); tile->Locations.pop_back();
				i--;

				continue; // donht spawn particle
			}

			if (die) {
				me.dX = iw::randf() * 5;
				me.dY = iw::randf() * 5;
				me.Props = CellProperties::MOVE_DOWN;

				if (Physical* p = Space->FindEntity(transform).Find<Physical>()) {
					me.dX += p->Velocity.x * deltaTime;
					me.dY += p->Velocity.y * deltaTime;
				}
			}

			chunk->SetCell(x, y, me, world.m_currentTick);
		}
	}

	void SandLayer::PasteTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			PasteTile(t, a);
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
				
				if (!chunk) continue;

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

	void SandLayer::Reset() {
		world.Reset();
		Space->Clear();
		m_minimapTexture->Clear();

		srand(iw::Ticks());

		float playerY = __chunkSize/2 - __worldSizeY - 10;

		GenerateWorld(playerY);

		nextLevelTimer = nextLevelTime;
		nextPlayerLocation = -1;

		player = Space->CreateEntity<iw::Transform, Tile, Player, Physical, SharedCellData>();

		player.Set<iw::Transform>(iw::vector2(0, playerY));
		player.Set<Tile>(std::vector<iw::vector2> {
			vector2(0, 4), vector2(1, 4), vector2(2, 4), vector2(3, 4), vector2(4, 4), vector2(5, 4), vector2(6, 4),
			vector2(0, 3), vector2(1, 3), vector2(2, 3), vector2(3, 3), vector2(4, 3), vector2(5, 3), vector2(6, 3),
			vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2), vector2(4, 2), vector2(5, 2), vector2(6, 2),
			               vector2(1, 1), vector2(2, 1), vector2(3, 1), vector2(4, 1), vector2(5, 1),  
			                              vector2(2, 0), vector2(3, 0), vector2(4, 0)
		}, 6);
		player.Set<Player>();
		Physical*       p = player.Set<Physical>();
		SharedCellData* s = player.Set<SharedCellData>();

		p->Speed = 0;
		s->RecordHitCells = true;
	}

	void SandLayer::GenerateWorld(float playerY) {
		int height = playerY + 360;

		int bar = 50;
		int snd = 200;

		float offset = iw::randf() * __worldSizeX;

		for (int i = -__worldSizeX; i < __worldSizeX + __chunkSize / __chunkScale; i++) {
			for (int h = 0; h < bar; h++) {
				world.SetCell(i, height + h, Cell::GetDefault(CellType::METAL));
			}

			for (int h = 0; h < snd; h++) {
				float perlin = iw::perlin(offset + float(i) / 100, float(h) / 100);

				Cell c;
				if (perlin > 0.25f) {
					c = Cell::GetDefault(CellType::ROCK);
				}

				else if (perlin > 0) {
					c = Cell::GetDefault(CellType::WATER);
				}

				else {
					c = Cell::GetDefault(CellType::SAND);
				}

				c.Gravitised = true;

				world.SetCell(i, bar + height + h, c);
			}
		}

		/*int island = __worldSizeX * 0.5f;
		
		float pScale = 0;
		float hScale = 0;
		float bias   = 0;*/

		//for (int i = -__worldSizeX; i < __worldSizeX; i++) {
		//	bool isIsland = abs(i) > island;

		//	float hs = isIsland ? 20  : 10;
		//	float ps = isIsland ? 250 : 10;
		//	float b  = isIsland ? __seaHeight : 25;

		//	hScale = i == -__worldSizeX ? hs : iw::lerp(hScale, hs, __stepTime);
		//	pScale = i == -__worldSizeX ? ps : iw::lerp(pScale, ps, __stepTime);
		//	bias   = i == -__worldSizeX ? b  : iw::lerp(bias,   b,  __stepTime);

		//	int height = (iw::perlin(float(i) / pScale) + 1) * hScale + bias;

		//	for (int h = 0; h < height; h++) {
		//		world.SetCell(i, h - __worldSizeY, Cell::GetDefault(CellType::SAND));
		//	}

		//	for (int h = height; h < __seaHeight; h++) {
		//		world.SetCell(i, h - __worldSizeY, Cell::GetDefault(CellType::WATER));
		//	}
		//}
	}

	void SandLayer::_SpawnCluster(
		float x, float y,
		float s, 
		int n)
	{
#ifdef _DEBUG
		for (auto [x, y] : FillCircle(x, y, s)) {
			Cell c = Cell::GetDefault(CellType::DEBUG);
			c.Color = iw::Color(0, 1, 0);
			c.Life = 1000;
			if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
		}
#endif

		for (int i = 0; i < n; i++) {
			iw::vector2 r = iw::vector2::random() * (iw::randf()+1) * s;
		
			float px = x + r.x;
			float py = y + r.y;

			/*if ((playerSpawn - iw::vector2(px, py)).length_squared() < __asize *__asize) {
				a -= step;
				continue;
			}*/

			iw::Entity asteroid = Space->CreateEntity<iw::Transform, SharedCellData, Physical, Asteroid>();
			iw::Transform*  t = asteroid.Set<iw::Transform>();
			SharedCellData* s = asteroid.Set<SharedCellData>();
			                    asteroid.Set<Physical>();

			t->Position.x = px;
			t->Position.y = py;

			s->pX = px;
			s->pY = py;

			float  size = 200 + iw::randf()*50;
			float hsize = size/2;
			float fsize = size/5;
			float isize = size/10;
			
#ifdef _DEBUG
			for (auto [x, y] : FillCircle(px, py, size)) {
				Cell c = Cell::GetDefault(CellType::DEBUG);
				c.Color = iw::Color(1, 1, 0);
				c.Life = 1000;
				if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
			}
#endif
			// make this looks like the processing version (more jaged)
			for(int i = -size; i < size; i++)
			for(int j = -size; j < size; j++) {
				float x = i; // thought there might need to be a remap but idk
				float y = j;

				float dist = sqrt(x*x + y*y);

				if (dist < (iw::perlin(px + x/hsize, py + y/hsize) + 1) * hsize) {
					Cell c;
					if (dist < (iw::perlin(px + x/isize, py + y/isize) + 1) * fsize)
 						 c = Cell::GetDefault(CellType::REZ);
					else c = Cell::GetDefault(CellType::ROCK);

					if (   world.InBounds(x + px, y + py)
						&& world.GetCell (x + px, y + py).Type == CellType::REZ)
					{
						continue;
					}

					c.Share = s;
					world.SetCell(x + px, y + py, c);
				}
			}
		}
	}
}
