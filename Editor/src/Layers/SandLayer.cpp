#include "Layers/SandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"

double __chunkScale = 2;
int __chunkSize = 200;
int __arenaSize = 100;
int __arenaSizeBuf = 5;
int __brushSizeX = 50;
int __brushSizeY = 50;
float __stepTime = 1/60.f;
float __slowStepTime = 10.f;
bool __stepDebug = false;

namespace iw {
	SandLayer::SandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(__chunkSize, __chunkSize, __chunkScale))
		, reset(true)
	{
		world.SetMaxChunks(__arenaSize, __arenaSize);

		__chunkSize /= __chunkScale;
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

		bullet.dX  = 750; // Initial speeds
		laser.dX   = 1500;
		elaser.dX  = 1000;
		mlaser.dX  = 2000;
		missile.dX = 200;

		debug.Life = 2/60.f;
		laser .Life  = 0.06f;
		elaser.Life  = 1.00f;
		//mlaser.Life  = 0.01f;
		bullet.Life  = 0.01f;
		missile.Life = 0.05f;
		explosn.Life = 0.06f;
		smoke.Life   = 0.5f;

		empty  .Precedence = 0;
		smoke  .Precedence = 1;
		explosn.Precedence = 100;

		rez    .Precedence = 1000;

		laser  .Precedence = 15;
		elaser .Precedence = 15;
		mlaser .Precedence = 1001;
		bullet .Precedence = 15;
		missile.Precedence = 15;

		bullet.MaxSplitCount = 2;
		laser .MaxSplitCount = 5;
		elaser.MaxSplitCount = 1;

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

		slowStepTimer -= iw::DeltaTime();
		if (slowStepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) {
			slowStepTimer = __slowStepTime;
			Space->Query<SharedCellData, Physical>().Each([&](auto, auto s, auto p) {
				p->Radius = s->Radius()*1.5f;
			});
			Space->Query<Tile, Physical>().Each([&](auto, auto a, auto p) {
				p->Radius = a->Radius()*5;
			});
		}

		stepTimer -=  iw::DeltaTime();
		if (stepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) {
			Space->Query<iw::Transform, Player, Physical>().Each([&](auto, auto t, auto p, auto pp) {
				if (p->Movement.y == 0) {
					pp->Speed = iw::lerp(pp->Speed, 0.f, deltaTime*2);
				}

				else if (p->Movement.z == 1 && p->BoostFuel > 0) {
					p->BoostFuel -= iw::DeltaTime();
					pp->Speed = iw::lerp(pp->Speed, 800.f, deltaTime*10); // acceleration
				}

				else {
					pp->Speed = iw::clamp<float>(pp->Speed + p->Movement.y * 10, -500, 500);
				}
				
				if (p->Movement.z == 0) {
					p->BoostFuel = iw::clamp<float>(p->BoostFuel + deltaTime/10, 0, p->MaxBoostFuel);
				}

				float rot = -p->Movement.x *  deltaTime;
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, rot);

				// Dont allow going out of bounds

				if (t->Position.length_squared() >= (__arenaSize-__arenaSizeBuf)*(__arenaSize-__arenaSizeBuf) * __chunkSize*__chunkSize) {
					float angle = atan2(t->Position.y, t->Position.x);
					t->Rotation = /*iw::lerp(t->Rotation, */iw::quaternion::from_euler_angles(0, 0, angle)/*, deltaTime)*/;
				}

				pp->Velocity = t->Up() * pp->Speed;
					
				// Bullets n such

				p->FireTimeout -= deltaTime;
				if (   p->FireTimeout < 0
					&& p->FireButtons != 0)
				{
					if (p->FireButtons.x == 1) {
						p->FireTimeout = 0.035f;
						Fire(t, pos, pp->Velocity, Cell::GetDefault(CellType::BULLET), true);
					}
					
					else if (p->FireButtons.y == 1) {
						p->FireTimeout = 0.001f;
						Fire(t, pos, 0, Cell::GetDefault(CellType::LASER), false);
					}

					else if (p->FireButtons.z == 1) {
						p->FireTimeout = 0.01f;
						vector2 d = t->Position + t->Right() * (iw::randf() > 0 ? 1 : -1);
						FireMissile(t, d, Cell::GetDefault(CellType::MISSILE));
					}

					p->FireButtons.z = 0; // need to reset because its on scroll wheel :/
				}
			});

			auto space = Space; // :<

			Space->Query<iw::Transform, Flocking, Physical>().Each([&, space](auto e, auto t, auto f, auto p) {
				if (!f->Active) return; // for missiles

				iw::vector2 avgAway = 0;
				iw::vector2 avgCenter = 0;
				iw::vector2 avgForward = 0;
				int count = 0;

				space->Query<iw::Transform, Physical>().Each([&](auto e2, auto t2, auto p2) {
					if (e == e2) return;

					iw::vector2 away = t->Position - t2->Position;
					if (away.length_squared() > p2->Radius*p2->Radius) {
						return;
					}

					for (auto [x, y] : FillCircle(t2->Position.x, t2->Position.y, p2->Radius)) {
						world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
					}

					avgAway    += away * p2->Radius / away.length();
					avgCenter  += t2->Position;
					avgForward += p2->Velocity;
					count      += 1;
				});

				if (count > 0) {
					avgAway    /= count;
					avgCenter  /= count; avgCenter -= t->Position;
					avgForward /= count;
				}
				
				if(p->HasTarget) {
					LOG_INFO << p->Target;

					iw::vector2 at = t->Position - p->Target;

					if (at.length_squared() < p->TargetRadius*p->TargetRadius) {
						avgAway += at * (p->AttractTarget ? -1 : 1)/2;
					}
				}

				p->Velocity = iw::lerp(p->Velocity, p->Velocity + avgAway /*+ avgCenter*/ + avgForward/10, deltaTime*5);

				// Target force
				iw::vector2 nVel = (p->Velocity).normalized();
				iw::vector2 nDir = (p->Target - t->Position).normalized();
				iw::vector2 delta = (nDir - nVel) * p->Speed * p->TurnRadius;

				p->Velocity = (p->Velocity + delta).normalized() * p->Speed;
			});

			Space->Query<iw::Transform, EnemyShip, Physical>().Each([&](auto e, auto t, auto s, auto p) {
				// Check for another command every few seconds

				s->RezLow      = s->Rez <= s->MinRez;
				s->AtObjective = (t->Position - s->Objective)  .length_squared() < 300*300;    // 300 meters form objective
				//s->AttackMode  = s->PlayerVisible; //(t->Position - playerLocation).length_squared() < 2500*2500; // 2500 meters from player

				p->AttractTarget = s->Homecoming;

				s->ChecekCommandTimer -= deltaTime;
				if (s->ChecekCommandTimer <= 0) {
					s->ChecekCommandTimer = s->ChecekCommandTime;
					s->Command->RequestObjective(e);
				}

				// Always move twoards objective

				if (s->AttackMode) {
					p->Speed = iw::lerp(p->Speed, s->Speed*2, iw::DeltaTime()); // fast when attacking
				}

				else if (s->AtObjective) {
					p->Speed = iw::lerp(p->Speed, s->Speed/5, iw::DeltaTime()); // slow at objective
				}

				else {
					p->Speed = iw::lerp(p->Speed, s->Speed, iw::DeltaTime()); // normal
				}
				
				p->Target = s->Objective;
			});

			Space->Query<iw::Transform, EnemyShip, EnemyAttackShip>().Each([&](auto, auto t, auto s, auto) {
				if (s->AttackMode) {
					s->FireTimer -= deltaTime;
					if (s->FireTimer <= 0) {
						s->FireTimer = s->FireTime * (iw::randf() + 1.1f);
						s->Rez -= s->RezToFireLaser;

						Fire(t, playerLocation, 0, Cell::GetDefault(CellType::eLASER), true); // should shoot forward, not just at player location
					}

					s->Objective = playerLocation;
				}
			});

			int supplyShipCout = 0;

			Space->Query<iw::Transform, EnemyShip, EnemySupplyShip>().Each([&, space](auto e, auto t, auto s, auto ss) {
				supplyShipCout++;

				SharedCellData* user = space->FindComponent<SharedCellData>(e); // not in query because it should be an option

				for (auto [coords, cell] : user->HitCells) {
					if (cell.Type == CellType::REZ) {
						ss->CapturedRez = iw::clamp(ss->CapturedRez + 1, 0, ss->MaxRez);
					}
				}

				iw::vector2 commandLocation = space->FindEntity(s->Command).Find<iw::Transform>()->Position;
				if (s->AttackMode) {
					iw::vector2 c = t->Position - commandLocation;
					iw::vector2 a = t->Position - playerLocation;

					if (c.length_squared() <= a.length_squared()) {
						s->WantGoHome = true;
					}
					else {
						s->Objective = (t->Position - playerLocation).normalized() * 3000; // try and get 3 km away
					}
				}

				else if (ss->CapturedRez >= ss->MaxRez) {
					s->WantGoHome = true; // go home
					// wait could put as not below, idk if there should be behaviour here
				}

				else if (s->AtObjective && s->Objective != commandLocation) {
					Cell p = Cell::GetDefault(CellType::LASER);
					p.Share = user;
					Fire(t, s->Objective, 0, p, -1);
				}

				if (!s->AtObjective && ss->CapturedRez >= ss->MaxRez) {
					s->ChecekCommandTimer = 3; // wait at command for 3 seconds
				}
			});

			Space->Query<iw::Transform, EnemyBase>().Each([&, space](auto e, auto t, auto b) {
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, deltaTime/60);

				b->EstPlayerLocation.z += iw::DeltaTime() * 2500; // Should be 5000km within 2 seconds

				for (iw::EntityHandle& entity : b->NeedsObjective) {
					EnemyShip* ship = Space->FindComponent<EnemyShip>(entity);
					if (!ship) continue;

					if (ship->AttackMode) { // If player is found
						b->EstPlayerLocation = iw::vector3(playerLocation, 0);
					}

					// send ship home if it has little rez, could do suicide mission if base is looking grim
					if ((ship->RezLow || ship->WantGoHome) && !ship->Homecoming) {
						ship->AtObjective = true;
						ship->Objectives.clear();
					}

					if (ship->AtObjective) {
						// send ship home if it has gotten to all objectives
						if (ship->Objectives.size() == 0) {
							ship->Homecoming = true;
							ship->Objectives.push_back(t->Position); // one of these gets removed, kinda dumb
							ship->Objectives.push_back(t->Position);
						}

						// send ship twoards player if est location is good enough 
						else if (   b->EstPlayerLocation.z < 2500
							&& space->HasComponent<EnemyAttackShip>(entity))
						{
							iw::vector2 v = b->EstPlayerLocation + b->EstPlayerLocation.z * ship->Objectives.back().normalized();
							ship->Objectives.push_back(v);
						}

						// give supplies to base and destory ship if at home base
						else if (ship->Homecoming && Space->HasComponent<EnemySupplyShip>(entity)) {
							EnemySupplyShip* supply = Space->FindComponent<EnemySupplyShip>(entity);
							b->Rez += ship->Rez + supply ? supply->CapturedRez : 0;
							b->UseRezTimer += 2; // take 2 seconds to 'process'

							space->DestroyEntity(entity);
							supplyShipCout--;

							continue;
						}

						// send ship to next objective
						ship->Objective = ship->Objectives.back(); ship->Objectives.pop_back();
					}
				}
				b->NeedsObjective.clear();

				b->UseRezTimer -= iw::DeltaTime();
				if (   b->UseRezTimer <= 0
					&& b->Rez > 0
					|| Keyboard::KeyDown(iw::L))
				{
					b->UseRezTimer = b->UseRezTime;

					// if attack ships
					if(    b->Rez >= b->AttackShipCost
						&& iw::randf() < 0)
					{
						float enemyCount = iw::clamp(iw::randi(4) + 3.f, 7.f, b->Rez/b->AttackShipCost);
						b->Rez -= b->AttackShipCost * enemyCount;
							
						std::vector<iw::vector2> objectives;
						for (int i = 0; i < 3 + iw::randf(); i++) {
							objectives.emplace_back(iw::randf() * 1000, iw::randf() * 1000);
						}

						for (int i = 0; i < enemyCount; i++) {
							iw::Entity attackShip = Space->CreateEntity<iw::Transform, Tile, EnemyShip, EnemyAttackShip, Physical, Flocking>();
							Tile* tile =        attackShip.Set<Tile>(std::vector<iw::vector2> {
												vector2(1, 0),				 vector2(3, 0),
								vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1),
								vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
								vector2(0, 3),                               vector2(3, 3),
								vector2(0, 4),                               vector2(3, 4)
							}, 3);
							iw::Transform* st = attackShip.Set<iw::Transform>(t->Position + 200*iw::vector2(iw::randf(), iw::randf()));
							EnemyShip*     s  = attackShip.Set<EnemyShip>(b);
							Physical*      p  = attackShip.Set<Physical>();
												attackShip.Set<Flocking>();

							s->Objectives = objectives;
							s->Objective  = s->Objectives.back();

							p->Velocity = (st->Position - t->Position).normalized() * s->Speed;
						}
					}

					// if supply ship
					else if (b->Rez >= b->SupplyShipCost || supplyShipCout < 3) {
						b->Rez -= b->SupplyShipCost;

						std::vector<iw::vector2> objectives; // only do this when needed
						space->Query<iw::Transform, SharedCellData, Asteroid>().Each([&](auto e, auto t, auto s, auto) {
							if (s->UserTypeCounts[CellType::REZ].first > 100) {
								objectives.push_back(s->UserTypeCounts[CellType::REZ].second);
							}
						});
					
						if(objectives.size() > 0) {
							supplyShipCout++;
							
							iw::Entity supplyShip = Space->CreateEntity<iw::Transform, Tile, EnemyShip, EnemySupplyShip, Physical, Flocking, SharedCellData>();
							Tile* tile =          supplyShip.Set<Tile>(std::vector<iw::vector2> {
								vector2(0, 0),				                 vector2(3, 0),
								vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1), vector2(4, 1), vector2(5, 1),
								vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2), vector2(4, 2), vector2(5, 2),
								vector2(0, 3),                               vector2(3, 3),
								vector2(0, 4)
							}, 5);
							iw::Transform*   st = supplyShip.Set<iw::Transform>();
							EnemyShip*       s  = supplyShip.Set<EnemyShip>(b);
							Physical*        p  = supplyShip.Set<Physical>();
							SharedCellData*  d  = supplyShip.Set<SharedCellData>();
							EnemySupplyShip* ss = supplyShip.Set<EnemySupplyShip>();
					   							  supplyShip.Set<Flocking>();

							tile->TileId = space->HasComponent<Tile>(e) ? space->FindComponent<Tile>(e)->TileId : 0;

							iw::vector2 f; // place ship in a spot where it wont be destroied
							do {
								st->Position = t->Position + 100 * iw::vector2(iw::randf(), iw::randf());
								f = st->Position + tile->FurthestPoint(st->Position - t->Position); // doesnt account for roataion
							} while (!world.IsEmpty(f.x, f.y));

							s->Objectives = objectives;
							s->Objective = s->Objectives.back();

							p->Velocity = (st->Position - t->Position).normalized() * s->Speed;

							d->RecordHitCells = true;
						}
					}
			
					b->FireTimer -= deltaTime;
					if (b->FireTimer > 0) {
						b->FireTimer = b->FireTime * (iw::randf() + 2);
						Fire(t, playerLocation, 0, Cell::GetDefault(CellType::LASER), false); // Main base will have to just be a gun bc no offset lol
					}
				}
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

			// Turn around objects getting close to outside arena

			Space->Query<iw::Transform, Physical>().Each([&](auto, auto t, auto p) {
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
				s->vX = p->Velocity.x;
				s->vY = p->Velocity.y;
			});
			Space->Query<SharedCellData>().Each([&](auto e, auto s) {
				s->pX += s->vX * deltaTime;
				s->pY += s->vY * deltaTime;

				s->HitCells.clear();
			});
			
			//Space->Query<iw::Transform, SharedCellData>().Each([&](auto e, auto t, auto s) {
			//	s->pX = t->Position.x;
			//	s->pY = t->Position.y;
			//});


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

			// Line of sight checks need to see Tiles
			Space->Query<iw::Transform, EnemyShip, Tile>().Each([&](auto, auto t, auto s, auto a) {
				iw::vector2 d = playerLocation - t->Position;
				d.normalize();
				d *= 600; // max sightline

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

			// Sand update

			// Paste tiles up where to keep tile cells after they die

							   PasteTiles();
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

			stepTimer = deltaTime;
		}

		m_stars.GetTransform()->Position = iw::vector2(-playerLocation.x, -playerLocation.y) / 5000;

		Renderer->BeginScene(MainScene);
			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);

			Renderer->BeforeDraw([&, playerLocation, deltaTime]() {
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
		//if (EnemyShip* ship = Space->FindComponent<EnemyShip>(e.Entity)) {
		//	iw::vector2 commandPosition = Space->FindEntity(ship->Command).Find<iw::Transform>()->Position;

		//	if (   ship->Homecoming     // At base
		//		&& ship->AtObjective
		//		&& ship->Objective == commandPosition)
		//	{ 
		//		EnemySupplyShip* supply = Space->FindEntity(ship).Find<EnemySupplyShip>();
		//		ship->Command->Rez += ship->Rez + supply ? supply->CapturedRez : 0;
		//	}
		//}

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
	
	void SandLayer::Reset() {
		world.Reset();
		Space->Clear();

		iw::vector2 playerSpawn = 0;

		player = Space->CreateEntity<iw::Transform, Tile, Player, Physical>();

		player.Set<iw::Transform>(playerSpawn);
		player.Set<Tile>(std::vector<iw::vector2> {
				               vector2(1, 4),
				vector2(0, 3), vector2(1, 3), vector2(2, 3), vector2(3, 3),
				vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
				vector2(0, 1),							     vector2(3, 1),
				vector2(0, 0),							     vector2(3, 0)
		}, 5);
		player.Set<Player>();
		Physical* p = player.Set<Physical>();

		p->Speed = 0;

		//// enemy base //

		iw::Entity e = Space->CreateEntity<iw::Transform, EnemyBase, Tile>();
		e.Set<iw::Transform>(iw::vector2(1000, 0));
		e.Set<EnemyBase>();
		e.Set<Tile>(std::vector<iw::vector2> {
				               vector2(1, 2),
				vector2(0, 1), vector2(1, 1), vector2(2, 1),
				               vector2(1, 0),                
		}, 100);

		//iw::Entity e1 = Space->CreateEntity<iw::Transform, EnemyBase, Tile>();
		//e1.Set<iw::Transform>(iw::vector2(600, 600));
		//e1.Set<EnemyBase>();
		//e1.Set<Tile>(std::vector<iw::vector2> {
		//	vector2(1, 2),
		//		vector2(0, 1), vector2(1, 1), vector2(2, 1),
		//		vector2(1, 0),
		//}, 75);

		// asteroid //

		srand(iw::Ticks());

		for (int i = 0; i < 20; i++) {
			iw::vector2 v(iw::randf(), iw::randf());
			v.normalize();
			v *= iw::randf() * 300;/* __arenaSize*__chunkSize;*/

			float xOff = v.x;
			float yOff = v.y;

			if ((playerSpawn - iw::vector2(xOff, yOff)).length_squared() < 250*250) {
				i--; continue;
			}

			iw::Entity asteroid = Space->CreateEntity<iw::Transform, SharedCellData, Physical, Asteroid>();
			iw::Transform*  t = asteroid.Set<iw::Transform>();
			SharedCellData* s = asteroid.Set<SharedCellData>();

			t->Position.x = xOff;
			t->Position.y = yOff;

			s->pX = xOff;
			s->pY = yOff;

			for(int i = -150; i < 150; i++)
			for(int j = -150; j < 150; j++) {
				float x = i; // thought there might need to be a remap but idk
				float y = j;

				float dist = sqrt(x*x + y*y);

				if (dist < (iw::perlin(x/70 + xOff, y/70 + yOff) + 1) * 75) {
					Cell c;
					if (dist < (iw::perlin(x/10 + xOff, y/10 + yOff) + 1) * 25) {
						c = Cell::GetDefault(CellType::REZ);
					}
					else {
						c = Cell::GetDefault(CellType::ROCK);
					}

					c.Share = s;
					world.SetCell(x + xOff, y + yOff, c);
				}
			}


		}

		Cell base;
		if (iw::randf() > 0) base = Cell::GetDefault(CellType::SMOKE);
		else				 base = Cell::GetDefault(CellType::ROCK);

		for (int i = 0; i < 1; i++) {
			iw::vector2 v(iw::randf(), iw::randf());
			v.normalize();
			v *= iw::randf() * 350; /* __arenaSize*__chunkSize;*/

			float xOff = v.x;
			float yOff = v.y;

			if ((playerSpawn - iw::vector2(xOff, yOff)).length_squared() < 250*250) {
				i--; continue;
			}

			iw::Entity asteroid = Space->CreateEntity<iw::Transform, SharedCellData, Physical, Asteroid>();
			iw::Transform*  t = asteroid.Set<iw::Transform>();
			SharedCellData* s = asteroid.Set<SharedCellData>();

			t->Position.x = xOff;
			t->Position.y = yOff;

			s->pX = xOff;
			s->pY = yOff;

			for(int i = 0; i < 900; i++) {
				iw::vector2 v2(iw::randf(), iw::randf());
				v2.normalize();
				v2 *= iw::randf() * 300;

				float x = v2.x; // see above
				float y = v2.y;

				if (!world.IsEmpty(x + xOff, y + yOff)) continue;

				if (sqrt(x*x + y*y) < (iw::perlin(x/70 + xOff, y/70 + yOff) + 1) * 150) {
					Cell c = base;

					c.dX = iw::randf();
					c.dY = iw::randf();

					int props = iw::val(c.Props);
					props &= ~iw::val(CellProperties::DELETE_TIME);
					c.Props = (CellProperties)props;

					c.Share = s;
					world.SetCell(x + xOff, y + yOff, c);
				}
			}
		}
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

		int minX = fx  - world.m_chunkWidth * 10;
		int maxX = fx2 + world.m_chunkWidth * 10;

		int minY = fy  - world.m_chunkHeight * 10;
		int maxY = fy2 + world.m_chunkHeight * 10;

		auto [minCX, minCY] = world.GetChunkLocation(minX, minY);
		auto [maxCX, maxCY] = world.GetChunkLocation(maxX, maxY);

		std::vector<SandChunk*> chunksUpdated;

		for (int px = 0; px < 2; px++)
		for (int py = 0; py < 2; py++) {
			for (int x = minCX + px; x < maxCX; x += 2)
			for (int y = minCY + py; y < maxCY; y += 2) {
				SandChunk* chunk = world.GetChunkDirect(x, y);

				if (!chunk) continue;

				chunksUpdated.push_back(chunk);

				if (chunk->IsAllEmpty()) continue;

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
		
		chunkCount = chunksUpdated.size();

		for (SandChunk* chunk : chunksUpdated) {
			Task->queue([&, chunk]() {
				chunk->CommitMovedCells(world.m_currentTick);

				{  std::unique_lock lock(chunkCountMutex); chunkCount--; }
				chunkCountCV.notify_one();
			});
		}

		{
			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

		return cellsUpdatedCount;
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

			if (   cell.Type   == CellType::EMPTY
				|| cell.TileId == tile->TileId
				|| cell.Precedence < tile->Precedence)
			{
				Cell me = Cell::GetDefault(CellType::METAL);
				me.TileId = tile->TileId;
				me.Life = 1;

				if (death) {
					me.dX = iw::randf()*5;
					me.dY = iw::randf()*5;
					me.Props = CellProperties::MOVE_DOWN;

					if (Physical* p = Space->FindEntity(transform).Find<Physical>()) {
						me.dX += p->Velocity.x * deltaTime;
						me.dY += p->Velocity.y * deltaTime;
					}
				}

				chunk->SetCell(x, y, me, world.m_currentTick);
			}

			else {
				tile->Locations[i] = tile->Locations.back(); tile->Locations.pop_back();
				i--;
			}
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
}
