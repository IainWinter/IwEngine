#include "Layers/SpaceSandLayer.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"
#include "iw/audio/AudioSpaceStudio.h"

#include "iw/math/matrix.h"

#include "iw/math/noise.h"

#include "iw/common/algos/MarchingCubes.h"

double __chunkScale = 1.5f * 2;
int __chunkSize = 150;
int __arenaSize = 50;
int __arenaSizeBuf = 1;
int __brushSizeX = 50;
int __brushSizeY = 50;
float __stepTime = 1/60.f;
float __slowStepTime = 10.f;
bool __stepDebug = false;

int __worldSize = -1; // gets set below

#undef _DEBUG

namespace iw {
	SpaceSandLayer::SpaceSandLayer()
		: Layer("Ray Marching")
		, m_sandTexture(nullptr)
		, world(SandWorld(__chunkSize, __chunkSize, __chunkScale))
		, reset(true)
	{
		world.SetMaxChunks(__arenaSize, __arenaSize);
		__worldSize = __arenaSize * __chunkSize / __chunkScale;
	}

	int SpaceSandLayer::Initialize() {
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
		//iw::ref<iw::Texture> bulletIconTex  = Asset->Load<iw::Texture>("textures/SpaceGame/bullet_icon.png");
		iw::ref<iw::Texture> missileIconTex = Asset->Load<iw::Texture>("textures/SpaceGame/missile_icon.png");
		energyIconTex ->SetFilter(iw::NEAREST);
		//bulletIconTex ->SetFilter(iw::NEAREST);
		missileIconTex->SetFilter(iw::NEAREST);

		iw::ref<iw::Material> energyIconMat  = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		//iw::ref<iw::Material> bulletIconMat  = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		iw::ref<iw::Material> missileIconMat = REF<iw::Material>(Asset->Load<Shader>("shaders/texture.shader"));
		energyIconMat ->SetTexture("texture", energyIconTex);
		//bulletIconMat ->SetTexture("texture", bulletIconTex);
		missileIconMat->SetTexture("texture", missileIconTex);
		energyIconMat ->SetTransparency(iw::Transparency::ADD);
		//bulletIconMat ->SetTransparency(iw::Transparency::ADD);
		missileIconMat->SetTransparency(iw::Transparency::ADD);

		m_energyIconScreen = Renderer->ScreenQuad().MakeInstance();
		//m_bulletIconScreen  = Renderer->ScreenQuad().MakeInstance();
		m_missileIconScreen = Renderer->ScreenQuad().MakeInstance();
		m_energyIconScreen .SetMaterial(energyIconMat);
		//m_bulletIconScreen .SetMaterial(bulletIconMat);
		m_missileIconScreen.SetMaterial(missileIconMat);

		iw::ref<iw::Material> energyMat = REF<iw::Material>(Asset->Load<Shader>("shaders/simple_color.shader"));
		//iw::ref<iw::Material> bulletMat = REF<iw::Material>(Asset->Load<Shader>("shaders/particle/simple_color.shader"));
		iw::ref<iw::Material> missileMat = REF<iw::Material>(Asset->Load<Shader>("shaders/particle/simple_color.shader"));
		energyMat ->Set("color", iw::Color::From255(0,   255,   0));
		//bulletMat ->Set("color", iw::Color::From255(196, 141,  37));
		missileMat->Set("color", iw::Color::From255(230, 230, 230));
		energyMat ->SetTransparency(iw::Transparency::ADD);
		//bulletMat ->SetTransparency(iw::Transparency::ADD);
		missileMat->SetTransparency(iw::Transparency::ADD);
		
		m_energyScreen = Renderer->ScreenQuad().MakeInstance();
		m_energyScreen.SetMaterial(energyMat);

		// bullet & missile particle system

		//iw::Mesh bulletMesh  = Renderer->ScreenQuad().MakeCopy();
		iw::Mesh missileMesh = Renderer->ScreenQuad().MakeCopy();
		
		//bulletMesh.Data()->TransformMeshData(iw::Transform(0, iw::vector2(2.5f, .1)));
		missileMesh.Data()->TransformMeshData(iw::Transform(0, iw::vector2(.1, 2.5f)));

		//bulletMesh.SetMaterial(bulletMat);
		missileMesh.SetMaterial(missileMat);

		//m_bullets.SetParticleMesh(bulletMesh);
		m_missiles.SetParticleMesh(missileMesh);

		//// UI Component ideas

		//struct UIConstraint {
		//	virtual float Value() = 0;
		//};

		//struct PixelConstraint : UIConstraint {
		//	float Pixels;

		//	PixelConstraint(float pixels) : Pixels(pixels) {}

		//	float Value() override {
		//		return Pixels;
		//	}
		//};

		//class UIComponent {
		//private:
		//	std::vector<UIComponent*> m_children;
		//	
		//	iw::Mesh m_mesh;
		//	iw::Transform m_transform;

		//	// Has a mesh?
		//	// Generates transform from constraints

		//public:
		//	void Add(UIComponent* child) {
		//		m_children.push_back(child);
		//	}

		//	void Rmeove(UIComponent* child) {
		//		auto itr = std::find(m_children.begin(), m_children.end(), child);
		//		if (itr != m_children.end()) {
		//			m_children.erase(itr);
		//		}
		//	}

		//	void GenerateTransform() {

		//	}
		//};

		//iw::Entity miniMap = Space->CreateEntity<iw::Transform, UIComponent>();

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

	void SpaceSandLayer::PostUpdate() {
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

		//slowStepTimer -= iw::DeltaTime();
		//if (slowStepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) { // do one per frame, make this a special function like One() or something
		//	slowStepTimer = __slowStepTime;

		//}

		stepTimer -= iw::DeltaTime();
		if (stepTimer <= 0 && (!__stepDebug || Keyboard::KeyDown(V))) {
			stepTimer = __stepTime;

	// NEW PROCESS

			// MoveTilesToPhysics() Move cells from inside of AABB with the same tileid into a field, scan field for polygon, make triangles
			Space->Query<iw::Transform, Tile>().Each([&](
				iw::EntityHandle e,
				iw::Transform* transform,
				Tile* tile)
			{
				tile->Initialize(transform, world, Asset); // exits if already inited

				auto [field, sizeX, sizeY] = tile->GetCurrentField();
				auto [offX, offY]          = tile->GetCurrentOffset();

				if (!field) return; // exit if tile hasent been pasted yet

				SandChunk* chunk = nullptr;

				for(size_t x = 0; x < sizeX; x++)
				for(size_t y = 0; y < sizeY; y++) {
					unsigned f = field[x + y * sizeX];
					
					if (f == 0) continue;

					CellType type = (CellType) (f & 0xff000000);
					unsigned indx =             f & 0x00ffffff;

					int px = ceil(transform->Position.x + x + offX);
					int py = ceil(transform->Position.y + y + offY);

					if (!chunk || !chunk->InBounds(px, py)) {
						chunk = world.GetChunk(px, py);
					}

					if (!chunk) continue;

					// might not even need this check, but is a good safeguard against disturbing nontiles
					if (!world.GetCell(px, py).TileId == tile->TileId) {
						tile->RemoveCell(indx, Renderer->ImmediateMode());
					}

					chunk->SetCellData_dirty(chunk->GetIndex(px, py), Cell::GetDefault(CellType::EMPTY)); // see comment from below version

					// this might not be needed if the later loop takes care of it??
					
					//else {
						//world.GetCell(px, py).dX = iw::randf()*5;
						//world.GetCell(px, py).dY = iw::randf()*5;
						//world.GetCell(px, py).Props = CellProperties::MOVE_DOWN;

					//}
				}

				//if (Keyboard::KeyDown(iw::Y)) {
				//	tile->EjectTriangle(0);
				//}
			});

	// GAME LOGIC

			// Update one radius per frame

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

				if (p->Radius > 1000) {
					LOG_INFO << "asd " << p->Radius;
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
				//dd = iw::clamp<iw::vector2>(dd, -100, 100);

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

				s->ChecekCommandTimer -= deltaTime;
				if (s->ChecekCommandTimer <= 0) {
					s->ChecekCommandTimer = s->ChecekCommandTime;
					s->Command->RequestObjective(e);
				}

				// Dont explode at base

				iw::vector2 commandLocation = space->FindEntity(s->Command).Find<iw::Transform>()->Position;
				if (   s->AtObjective
					&& s->Objective == commandLocation)
				{
					if (Tile* tile = Space->FindComponent<Tile>(e)) {
						tile->ExplodeOnDeath = false;
					}
				}

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

			int supplyShipCout = 0;

			Space->Query<iw::Transform, EnemyShip, EnemySupplyShip>().Each([&, space](auto e, auto t, auto s, auto ss) {
				supplyShipCout++;

				SharedCellData* user = space->FindComponent<SharedCellData>(e); // not in query because it should be an option

				// Capture rez
				for (auto [coords, cell] : user->HitCells) {
					if (cell.Type == CellType::REZ) {
						ss->CapturedRez = iw::clamp(ss->CapturedRez + 1, 0, ss->MaxRez);
					}
				}

				// flee from player
				iw::vector2 commandLocation = space->FindEntity(s->Command).Find<iw::Transform>()->Position;
				if (s->AttackMode) {
					iw::vector2 pv = t->Position - playerLocation;

					iw::vector2 c = t->Position - commandLocation;
					iw::vector2 a = t->Position + pv.normalized() * 500;

					if (pv.length_squared() < 200*200) {
						if (ss->CapturedRez >= ss->MaxRez || c.length_squared() <= a.length_squared()) {
							s->WantGoHome = true;
						}

						else {
							s->Objective = a; // try and get 500 m away
						}
	
						// fire at player if 200 m of less
						s->FireDirection = iw::lerp(s->FireDirection, playerLocation, deltaTime*2);
						if (s->FireDirection.dot(playerLocation) > playerLocation.length_squared()*0.75f) {
							Fire(t, s->FireDirection, 0, Cell::GetDefault(CellType::mLASER), -1);
						}
					}
				}

				// If max rez go home
				else if (ss->CapturedRez >= ss->MaxRez) {
					s->WantGoHome = true; // go home
					// wait could put as not below, idk if there should be behaviour here
				}

				// If at objective and its not home, start shooting
				else if (s->AtObjective && s->Objective != commandLocation) {
					Cell p = Cell::GetDefault(CellType::mLASER);
					p.Share = user;
					Fire(t, s->Objective , 0, p, -1);
				}

				// While going home, delay for a command allow for a lil 'animation'
				if (!s->AtObjective && s->WantGoHome) {
					s->ChecekCommandTimer = 3;
				}

#ifdef _DEBUG
				for (auto [x, y] : FillCircle(s->Objective.x, s->Objective.y, 100)) {
					if (world.IsEmpty(x, y)) world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
				}
#endif
			});

			Space->Query<iw::Transform, EnemyBase>().Each([&, space](auto e, auto t, auto b) {
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, deltaTime);

				b->EstPlayerLocation.z += deltaTime * 2500; // Should be 5000km within 2 seconds

				for (iw::EntityHandle entity : b->NeedsObjective) {
					EnemyShip* ship = Space->FindComponent<EnemyShip>(entity);
					if (!ship) continue;

					// Update est player location if found
					if (ship->AttackMode) {
						b->EstPlayerLocation = iw::vector3(playerLocation, 0);
					}

					// send ship home if it has little rez, could do suicide mission if base is looking grim
					if ((ship->RezLow || ship->WantGoHome) && !ship->Homecoming) {
						ship->AtObjective = true;
						ship->Objectives.clear();
					}

					// Update objectives for supply ships not going home
					else if (space->HasComponent<EnemySupplyShip>(entity)) {
						iw::vector2 closest;
						float minDist = FLT_MAX;
						space->Query<iw::Transform, SharedCellData, Asteroid>().Each([&](auto e, auto t, auto s, auto) {
							auto [count, location] = s->UserTypeCounts[CellType::REZ];

							iw::vector2 to = space->FindComponent<iw::Transform>(entity)->Position - location;

							if (count > 100) {
								float dist = to.length_squared();
								if (dist < minDist) {
									closest = location;
									minDist = dist;
								}
							}
						});

						if (minDist == FLT_MAX) {
							ship->AtObjective = true;
							ship->Objectives.clear();
						}
						else {
							ship->Objectives = { closest }; // one gets removed
							ship->Objective = closest; // supply ships dont need a list of objectives
						}
					}

					// send attack ships twoards player if est location is good enough 
					else if (space->HasComponent<EnemyAttackShip>(entity)
						  && b->EstPlayerLocation.z < 2500)
					{
						iw::vector2 v = b->EstPlayerLocation + b->EstPlayerLocation.z * iw::vector2::random();
						ship->Objectives = { v };
						ship->AtObjective = true;
					}

					if (ship->AtObjective) {
						// send ship home if it has gotten to all objectives
						if (ship->Objectives.size() == 0) {
							ship->Homecoming = true;
							ship->Objectives.push_back(t->Position); // one of these gets removed, kinda dumb
							ship->Objectives.push_back(t->Position);
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

				//b->Rez += deltaTime;

				b->UseRezTimer -= deltaTime;
				if (   false && b->UseRezTimer <= 0
					//&& b->Rez > 0
					|| Keyboard::KeyDown(iw::L))
				{
					if ((playerLocation - t->Position).length_squared() < 1000 * 1000) {
						b->UseRezTimer = 1;
					}

					else {
						b->UseRezTimer = b->UseRezTime;
					}

					int minGroup = 3;

					// if attack ships
					if(    b->Rez >= b->AttackShipCost* minGroup
						&& iw::randf() < 0)
					{
						float enemyCount = iw::clamp(iw::randi(4) + 3, minGroup, b->Rez/b->AttackShipCost);
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

							tile->TileId = space->HasComponent<Tile>(e) ? space->FindComponent<Tile>(e)->TileId : 0; // put things like thses in common spawn

							s->Objectives = objectives;
							s->Objective  = s->Objectives.back();
							s->Speed = 150;

							p->Velocity = (st->Position - t->Position).normalized() * s->Speed;
						}
					}

					// if supply ship
					else if (b->Rez >= b->SupplyShipCost || supplyShipCout < 3) {
						if(b->Rez >= b->SupplyShipCost) {
							b->Rez -= b->SupplyShipCost;
						}

						int asterCount = 0;
						space->Query<iw::Transform, SharedCellData, Asteroid>().Each([&](auto e, auto t, auto s, auto) {
							asterCount++;
						});
					
						if(asterCount > 0) {
							supplyShipCout++;
							
							iw::Entity supplyShip = Space->CreateEntity<iw::Transform, Tile, EnemyShip, EnemySupplyShip, Physical, Flocking, SharedCellData>();
							Tile* tile =          supplyShip.Set<Tile>(std::vector<iw::vector2> {
								vector2(0, 0),				                 vector2(3, 0),
								vector2(0, 1), vector2(1, 1), vector2(2, 1), vector2(3, 1), vector2(4, 1), vector2(5, 1),
								vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2), vector2(4, 2), vector2(5, 2),
								vector2(0, 3),                               vector2(3, 3),
								vector2(0, 4)
							}, 7);
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
							} while (!world.IsEmpty(f.x, f.y) && world.InBounds(f.x, f.y));

							s->ChecekCommandTime = 1;
							s->Speed = 75;

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
					space->Query<iw::Transform, EnemyBase>().Each([&](auto, auto t2, auto) {
						iw::vector2 v = t2->Position - mpos;
						if (v.length_squared() < minDist) {
							minDist = v.length_squared();
							closest = t2->Position;
						}
					});

					p->HasTarget = false;

					if (minDist < 2500) {
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

			// Player

			Space->Query<iw::Transform, Player, Physical>().Each([&](auto e, auto t, auto p, auto pp) {
				// boost
				
				float speed = 250;

				if (   p->Movement.z == 1
					&& p->Energy > 0)
				{
					p->Energy -= 10 * deltaTime;
					speed = 750.f;								// speed is acceleration
				}

				int jerkTime = p->Movement.y == 0 ? 10 : 2;

				pp->Speed = iw::clamp(iw::lerp(pp->Speed, p->Movement.y * speed, deltaTime/jerkTime), 0.f, speed);
				
				float rot = -p->Movement.x * deltaTime;
				t->Rotation *= iw::quaternion::from_euler_angles(0, 0, rot);
				
				// Dont allow going out of bounds, different from others because velocity assignment, but overwrite or something below

				if (t->Position.length_squared() >= (__arenaSize-__arenaSizeBuf)*(__arenaSize-__arenaSizeBuf) * __chunkSize*__chunkSize) {
					float angle = atan2(t->Position.y, t->Position.x);
					t->Rotation = /*iw::lerp(t->Rotation, */iw::quaternion::from_euler_angles(0, 0, angle)/*, deltaTime)*/;
				}

				// Target force, make own funciton in iwmath, i guess a turn twoards
				iw::vector2 nVel = (pp->Velocity).normalized();
				iw::vector2 nDir = t->Up();
				iw::vector2 delta = (nDir - nVel) * pp->Speed * pp->TurnRadius;

				pp->Velocity = (pp->Velocity + delta).normalized() * abs(pp->Speed);

#ifdef _DEBUG
				for (auto [x, y] : FillLine(t->Position.x, t->Position.y, t->Position.x + pp->Velocity.x, t->Position.y + pp->Velocity.y)) {
					if (world.IsEmpty(x, y)) world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
				}

				for (auto [x, y] : FillLine(t->Position.x, t->Position.y, t->Position.x + t->Up().x*pp->Speed, t->Position.y + t->Up().y*pp->Speed)) {
					Cell c = Cell::GetDefault(CellType::DEBUG);
					c.Color = iw::Color(1, 1, 0);
					if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
				}
#endif

				// Bullets n such

				iw::vector2 target = pos;

				//if (targeting) {
				//	iw::vector2 P0 = targetPos;
				//	iw::vector2 P1 = playerLocation;
				//	iw::vector2 V0 = targetVel;

				//	float s0 = V0.length();
				//	float s1 = Cell::GetDefault(CellType::BULLET).Speed();

				//	float a =      (V0.x * V0.x) + (V0.y * V0.y) - (s1 * s1);
				//	float b = 2 * ((P0.x * V0.x) + (P0.y * V0.y) - (P1.x * V0.x) - (P1.y * V0.y));
				//	float c =      (P0.x * P0.x) + (P0.y * P0.y) + (P1.x * P1.x) + (P1.y * P1.y) - (2 * P1.x * P0.x) - (2 * P1.y * P0.y);

				//	float t1 = (-b + sqrt((b * b) - (4 * a * c))) / (2 * a);
				//	float t2 = (-b - sqrt((b * b) - (4 * a * c))) / (2 * a);

				//	t1 = t1 > 0 ? t1 : FLT_MAX;
				//	t2 = t2 > 0 ? t2 : FLT_MAX;

				//	float t = t1 < t2 ? t1 : t2;

				//	if(t != FLT_MAX) {
				//		target = P0 + (P0 - P1 + (t * s0 * V0)) / (t * s1);

				//		for (auto [x, y] : FillLine(P1.x, P1.y, target.x, target.y)) {
				//			if (world.IsEmpty(x, y)) world.SetCell(x, y, Cell::GetDefault(CellType::DEBUG));
				//		}
				//	}
				//}

				SharedCellData* user = space->FindComponent<SharedCellData>(e);
				Tile*           tile = space->FindComponent<Tile>(e);
				
				
				// Capture rez to make missiles
				for (auto [coords, cell] : user->HitCells) {
					if (cell.Type == CellType::REZ)
					{
						if (   p->MissileAmmo < p->MaxMissileAmmo
							&& iw::randf() > .99)
						{
							p->MissileAmmo += 1;
						}

						size_t r = iw::randi(tile->FullLocations.size());

						if (std::find(tile->Locations.begin(), tile->Locations.end(), tile->FullLocations[r])
							== tile->Locations.end())
						{
							tile->Locations.push_back(tile->FullLocations[r]);
						}
					}
				}

				p->FireTimeout -= deltaTime;
				if (   p->FireTimeout <  0
					&& p->FireButtons != 0)
				{
					if (   p->FireButtons.x == 1
						/*&& p->BulletAmmo > 0*/)
					{
						p->FireTimeout = 0.025;
						//p->BulletAmmo--;
						
						Fire(t, target +iw::vector2(iw::randf(), iw::randf())*5, pp->Velocity, Cell::GetDefault(CellType::BULLET), true);
					}
					
					else if (p->FireButtons.y == 1
						  && p->Energy > 0)
					{
						p->FireTimeout = 0.001f;
						p->Energy -= deltaTime * 33;

						Cell p = Cell::GetDefault(CellType::mLASER);
						p.Share = user;

						Fire(t, target, 0, p, false);
					}

					else if (p->FireButtons.z == 1
						  && p->MissileAmmo > 0)
					{
						p->FireTimeout = 0.01f;
						p->MissileAmmo--;

						vector2 d = t->Position + t->Right() * (iw::randf() > 0 ? 1 : -1);
						FireMissile(t, d, Cell::GetDefault(CellType::MISSILE));
					}

					p->FireButtons.z = 0; // need to reset because its on scroll wheel :/
				}

				// recharge bullets

				else if (p->FireTimeout <= -0.25f) {
					//if (   p->FireTimeout < -0.5f
					//	&& p->FireButtons.x == 0
					//	&& p->BulletAmmo < p->MaxBulletAmmo)
					//{
					//	p->BulletAmmo += 1;
					//	if (p->BulletAmmo % 5 == 0) {
					//		p->FireTimeout = -0.25f;
					//	}
					//}

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

	// PHYSICS & SAND STEP    Physics->Step()      Run physics

			// UpdateSandWorld()    Run sand
			updatedCellCount = UpdateSandWorld(fx, fy, fx2, fy2, deltaTime);

	// PASTE PHYSICS OBJECTS BACK PasteTiles()         Move cells from field back into the world, on collision eject cells
			Space->Query<iw::Transform, Tile>().Each([&](
				iw::EntityHandle e,
				iw::Transform* transform,
				Tile* tile)
			{
				float angle = transform->Rotation.euler_angles().z;

				tile->Update(angle, Renderer->ImmediateMode());

				auto [field, sizeX, sizeY] = tile->GetCurrentField();
				auto [offX, offY]          = tile->GetCurrentOffset();

				Cell c = Cell::GetDefault(CellType::METAL);
				c.TileId = tile->TileId;

				SandChunk* chunk = nullptr;

				for(size_t x = 0; x < sizeX; x++)
				for(size_t y = 0; y < sizeY; y++) {
					unsigned f = field[x + y * sizeX];
					
					if (f == 0) continue;

					CellType type = (CellType) (f & 0xff000000);
					unsigned indx =             f & 0x00ffffff;

					int px = ceil(transform->Position.x + x + offX);
					int py = ceil(transform->Position.y + y + offY);

					if (!chunk || !chunk->InBounds(px, py)) {
						chunk = world.GetChunk(px, py);
					}

					if (!chunk) continue;

					if (!chunk->IsEmpty(px, py)) {
						tile->RemoveCell(indx, Renderer->ImmediateMode());
					}

					chunk->SetCellData_dirty(chunk->GetIndex(px, py), c); // check presedense

					//else {
						//world.GetCell(px, py).dX = iw::randf() * 5;
						//world.GetCell(px, py).dY = iw::randf() * 5;
						//world.GetCell(px, py).Props = CellProperties::MOVE_DOWN;

						//world.SetCell(transform->Position.x + x, transform->Position.y + y, Cell::GetDefault(CellType::METAL));
					//}
				}
			});

	// RENDER UpdateSandTexture()
			UpdateSandTexture(fx, fy, fx2, fy2);

			// Render

			// END NEW PROCESS


			// old Sand update

			// Paste tiles up where to keep tile cells after they die
							 // PasteTiles();
			// Line of sight checks need to see Tiles
			
			//Space->Query<iw::Transform, EnemyShip, Tile>().Each([&](auto, auto t, auto s, auto a) {
			//	iw::vector2 d = playerLocation - t->Position;
			//	d.normalize();
			//	d *= 1000; // max sightline
//
			//	auto cells = RayCast(t->Position.x, t->Position.y, t->Position.x + d.x, t->Position.y + d.y);
//
			//	for (const Cell* c : cells) {
			//		if (c->TileId == a->TileId) continue;
			//		
			//		if (c->TileId == 0) {
			//			s->PlayerVisible = false;
			//			break;
			//		}
//
			//		if (c->TileId == player.Find<Tile>()->TileId) {
			//			s->PlayerVisible = true;
			//			break;
			//		}
			//	}
			//});

			//updatedCellCount = UpdateSandWorld  (fx, fy, fx2, fy2, deltaTime);
			                  // RemoveTiles();

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

		//int playerBulletAmmo  = player ? player.Find<Player>()->BulletAmmo : 0;
		int playerMissileAmmo = player ? player.Find<Player>()->MissileAmmo : 0;
		float energy          = player ? player.Find<Player>()->Energy : 0;
		float maxEnergy       = player ? player.Find<Player>()->MaxEnergy : 0; // need cus its a %

		// Render

		//while (playerBulletAmmo  != m_bullets .ParticleCount()) {
		//	if (playerBulletAmmo > m_bullets.ParticleCount()) {
		//		
		//		int x = m_bullets.ParticleCount() / 5;
		//
		//		m_bullets.SpawnParticle(iw::Transform(
		//			iw::vector2(
		//				x * 6,
		//				m_bullets.ParticleCount() - x * 5
		//			) + iw::vector2(2.5, .1)
		//		));
		//	}
		//
		//	else {
		//		m_bullets.DeleteParticle(m_bullets.ParticleCount() - 1);
		//	}
		//}
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

			// UI

			// can scale by pixels or % of screen
			iw::vector2 pixels(1.0f / Renderer->Width(), 1.0f / Renderer->Height());
			iw::vector2 aspect(1.0f, float(Renderer->Width()) / Renderer->Height());
																							 //iw::vector2 aspectY(float(Renderer->Height()) / Renderer->Width(), 1); // could be useful
			
			// minimap

			iw::vector2 mapPos(1, -1);
			iw::vector2 mapScale = .15 * aspect;

			if (Keyboard::KeyDown(iw::M)) {
				mapScale *= 2;
				m_minimapScreen.Material()->Set("textureScale", 3.f);
			}
			else {
				m_minimapScreen.Material()->Set("textureScale", 8.f);
			}

			Renderer->DrawMesh(iw::Transform(iw::vector3(mapPos + iw::vector2(-mapScale.x, mapScale.y), -1), mapScale), m_minimapScreen);

			float hide = 0;

			if (Keyboard::KeyDown(iw::H)) {
				hide = 300;
			}

			// tele

			float teleZ = -.05;

			iw::vector3 telePos = iw::vector2(-1, -1) - iw::vector2(100, 1000 + hide)*pixels;
			iw::vector3 teleScale = 600 * pixels;

			Renderer->DrawMesh(iw::Transform(iw::vector3(telePos + teleScale, teleZ), teleScale), m_teleScreen);

			// energy

			float iconZ = -0.1f;

			iw::vector2 dispScale  =     16*pixels;
			iw::vector2 iconScale  =     48*pixels;
			iw::vector2 iconPos    = -1 + 8*pixels;
			float       iconMargin =      8*pixels.y;

			iw::vector2 eiScale = iconScale/2;

			iw::vector2 eiPos = iconPos + iw::vector2(iconScale.x, -hide);
			iw::vector2 biPos = iconPos + iw::vector2(0, 2*  eiScale.y + iconMargin - hide);
			iw::vector2 miPos = biPos;//biPos + iw::vector2(0, 2 * iconScale.y + iconMargin - hide);

			Renderer->DrawMesh(iw::Transform(iw::vector3(eiPos + eiScale,   iconZ), eiScale),   m_energyIconScreen);
			//Renderer->DrawMesh(iw::Transform(iw::vector3(biPos + iconScale, iconZ), iconScale), m_bulletIconScreen);
			Renderer->DrawMesh(iw::Transform(iw::vector3(miPos + iconScale, iconZ), iconScale), m_missileIconScreen);

			iw::vector2 ePos = eiPos + iw::vector2(eiScale.x*2 + iconMargin, eiScale.y/2 - hide);
			//iw::vector2 bPos = biPos + iw::vector2(iconScale.x * 2, 0 - hide);
			iw::vector2 mPos = miPos + iw::vector2(iconScale.x * 2, 0 - hide);

			iw::vector2 eScale = dispScale * iw::vector2(energy/maxEnergy * 25, 1);

			Renderer->DrawMesh(iw::Transform(iw::vector3(ePos + eScale,    iconZ), eScale),    m_energyScreen);
			//Renderer->DrawMesh(iw::Transform(iw::vector3(bPos + dispScale, iconZ), dispScale), m_bullets.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(iw::vector3(mPos + dispScale, iconZ), dispScale), m_missiles.GetParticleMesh());

			// Debug

			Renderer->BeforeDraw([&, playerLocation, deltaTime]() {
				std::stringstream sb;
				sb << updatedCellCount << "\n" << deltaTime << "\n" << playerLocation << "\n" << mousePos;
				m_font->UpdateMesh(m_textMesh, sb.str(), 0.001, 1);
			});
			Renderer->DrawMesh(iw::Transform(-.4f), m_textMesh);

			// Main render

			Renderer->DrawMesh(m_stars.GetTransform(), &m_stars.GetParticleMesh());
			Renderer->DrawMesh(iw::Transform(), m_sandScreen);

		Renderer->EndScene();
	}

	bool SpaceSandLayer::On(
		MouseMovedEvent& e)
	{
		mousePos = { e.X, e.Y };

		if (!player) return false;
		player.Find<Player>()->MousePos = mousePos;

		return false;
	}

	bool SpaceSandLayer::On(
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

	bool SpaceSandLayer::On(
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
		
		if (EnemyBase* base = Space->FindComponent<EnemyBase>(e.Entity)) {
			EnemyBase* fallback = nullptr;
			Space->Query<EnemyBase>().Each([&](auto, auto fallbackBase) {
				if (fallbackBase != base) {
					fallback = fallbackBase;
				}
			});
			
			if (fallback == nullptr) {
				LOG_INFO << "You win";
				return false;
			}

			Space->Query<EnemyShip>().Each([&](auto, auto ship) {
				if (ship->Command == base) {
					ship->Command = fallback;
				}
			});
		}

		return false;
	}

	bool SpaceSandLayer::On(
		MouseWheelEvent& e)
	{
		if (!player) return false;
		Player* p = player.Find<Player>();
		p->FireButtons.z = e.Delta == 0 ? 0 : 1;
	}

	bool SpaceSandLayer::On(
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

	int SpaceSandLayer::UpdateSandWorld(
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
				cellsUpdatedCount += (chunk->m_maxX-chunk->m_minX)* (chunk->m_maxY - chunk->m_minY);

				Task->queue([&, chunk]() {
					DefaultSandWorker(world, *chunk, Space, deltaTime).UpdateChunk();

					{ std::unique_lock lock(chunkCountMutex); chunkCount--; }
					chunkCountCV.notify_one();
				});
			}

			// Wait for batch to finish
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

		// Wait for commits
		{
			std::unique_lock lock(chunkCountMutex);
			chunkCountCV.wait(lock, [&]() { return chunkCount == 0; });
		}

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

	void SpaceSandLayer::UpdateSandTexture(
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
	
	void SpaceSandLayer::PasteTile(iw::Transform* transform, Tile* tile, bool death, float deltaTime) {
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

				continue; // dont spawn particle
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

	void SpaceSandLayer::PasteTiles() {
		Space->Query<iw::Transform, Tile>().Each([&](
			auto e,
			auto t,
			auto a)
		{
			PasteTile(t, a);
		});
	}
	
	void SpaceSandLayer::RemoveTiles() {
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

	void SpaceSandLayer::Reset() {
		world.Reset();
		Space->Clear();
		m_minimapTexture->Clear();

		srand(iw::Ticks());

		GenerateWorld();
	}

	void SpaceSandLayer::GenerateWorld() {
		goto player;

		float __arenaRadius = (__arenaSize - __arenaSizeBuf) * __chunkSize;
		float __asteroidRadius = 150;

		iw::vector2 v = iw::vector2::random() * __arenaRadius;
		float r = (((iw::randf()+1)/2)*.7 + .7) * __arenaRadius;

#ifdef _DEBUG
		for (auto [x, y] : FillCircle(0, 0, __arenaRadius)) {
			Cell c = Cell::GetDefault(CellType::DEBUG);
			c.Color = iw::Color(1, 1, 1);
			c.Life = 1000;
			if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
		}

		for (auto [x, y] : FillCircle(v.x, v.y, r)) {
			Cell c = Cell::GetDefault(CellType::DEBUG);
			c.Color = iw::Color(1, 1, 0);
			c.Life = 1000;
			if (world.IsEmpty(x, y)) world.SetCell(x, y, c);
		}
#endif

		for (float a = 0; a < iw::Pi2; a += __asteroidRadius*2*2/r) { // arc of asteroid belt
			float x = v.x + cos(a) * r;
			float y = v.y + sin(a) * r;

			if (x*x + y*y > __arenaRadius*__arenaRadius) {
				continue;
			}

			_SpawnCluster(x, y, __asteroidRadius*2, 6);
		}

		iw::vector2 avgEnemyBaseLocation;

		for (int i = 0; i < 3; i++) { // random clusters
			iw::vector2 clusterLocation = iw::vector2::random() * (iw::randf()* __arenaRadius/3+__arenaRadius/2);
			float clusterCount = iw::randi(25) + 25;
			float clusterSize = (iw::randf()+2) * __asteroidRadius * clusterCount/10;

			_SpawnCluster(clusterLocation.x, clusterLocation.y, clusterSize, clusterCount);

			//// enemy base //

			iw::vector2 baseLocation = clusterLocation + iw::vector2::random()*clusterCount*1.5f;
			float baseSize = 100;

			// try to not destroy base on spawn, not perfect should check in circles
			while (!world.IsEmpty(baseLocation.x, baseLocation.y)) {
				baseLocation = clusterLocation + iw::vector2::random()*clusterSize*1.5f;
			}

			avgEnemyBaseLocation += baseLocation;

			iw::Entity e = Space->CreateEntity<iw::Transform, EnemyBase, Tile>();
			e.Set<iw::Transform>(baseLocation);
			e.Set<EnemyBase>();
			e.Set<Tile>(std::vector<iw::vector2> {
								   vector2(1, 2),
					vector2(0, 1), vector2(1, 1), vector2(2, 1),
								   vector2(1, 0),                
			}, 50);
		}

		//avgEnemyBaseLocation /= 3;
		//avgEnemyBaseLocation.normalize();
		//avgEnemyBaseLocation *= __arenaRadius;
	
	player:
		iw::vector3 playerLocation = 0;// -avgEnemyBaseLocation;

		//while (!world.IsEmpty(playerLocation.x, playerLocation.y)) // try to not destroy player on spawn, not perfect should check in circles
		//{
		//	playerLocation += iw::vector2::random()*__asteroidRadius;
		//}

		//temp
		iw::Entity e = Space->CreateEntity<iw::Transform, EnemyBase, Tile>();
		e.Set<iw::Transform>(100);
		e.Set<EnemyBase>();
		e.Set<Tile>(std::vector<iw::vector2> {
				//				vector2(0, 2),
				///*vector2(0, 1),*/ vector2(0, 1), /*vector2(2, 1),*/
							vector2(0, 0),                
		}, 30);
		//end temp

		player = Space->CreateEntity<iw::Transform, Tile, Player, Physical, SharedCellData>();

		player.Set<iw::Transform>(playerLocation);
		player.Set<Tile>(std::vector<iw::vector2> {
				               vector2(1, 4), vector2(2, 4),
				vector2(0, 3), vector2(1, 3), vector2(2, 3), vector2(3, 3),
				vector2(0, 2), vector2(1, 2), vector2(2, 2), vector2(3, 2),
				vector2(0, 1),							     vector2(3, 1),
				vector2(0, 0),							     vector2(3, 0),
		}, 6);
		player.Set<Player>();
		Physical*       p = player.Set<Physical>();
		SharedCellData* s = player.Set<SharedCellData>();

		p->Speed = 0;

		s->RecordHitCells = true;

		// Dust

		//Cell base;
		//if (iw::randf() > 0) base = Cell::GetDefault(CellType::SMOKE);
		//else				 base = Cell::GetDefault(CellType::ROCK);

		//for (int i = 0; i < 1; i++) {
		//	iw::vector2 v(iw::randf(), iw::randf());
		//	v.normalize();
		//	v *= iw::randf() * 350; /* __arenaSize*__chunkSize;*/

		//	float xOff = v.x;
		//	float yOff = v.y;

		//	iw::Entity asteroid = Space->CreateEntity<iw::Transform, SharedCellData, Physical, Asteroid>();
		//	iw::Transform*  t = asteroid.Set<iw::Transform>();
		//	SharedCellData* s = asteroid.Set<SharedCellData>();
		//	Physical*       p = asteroid.Set<Physical>();

		//	t->Position.x = xOff;
		//	t->Position.y = yOff;

		//	s->pX = xOff;
		//	s->pY = yOff;

		//	p->Solid = false;

		//	for(int i = 0; i < 900; i++) {
		//		iw::vector2 v2(iw::randf(), iw::randf());
		//		v2.normalize();
		//		v2 *= iw::randf() * 300;

		//		float x = v2.x; // see above
		//		float y = v2.y;

		//		if (!world.IsEmpty(x + xOff, y + yOff)) continue;

		//		if (sqrt(x*x + y*y) < (iw::perlin(x/70 + xOff, y/70 + yOff) + 1) * 150) {
		//			Cell c = base;

		//			c.dX = iw::randf();
		//			c.dY = iw::randf();

		//			int props = iw::val(c.Props);
		//			props &= ~iw::val(CellProperties::DELETE_TIME);
		//			c.Props = (CellProperties)props;

		//			c.Share = s;
		//			world.SetCell(x + xOff, y + yOff, c);
		//		}
		//	}
		//}
	}

	void SpaceSandLayer::_SpawnCluster(
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
