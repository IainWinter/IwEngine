#include "iw/engine/EntryPoint.h"

#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/Camera/LerpCameraControllerSystem.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/engine/Components/Timer.h"

#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/Collision/HullCollider.h"
#include "iw/physics/Collision/SphereCollider.h"
#include "iw/physics/Collision/CapsuleCollider.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/Mechanism.h"

#include "iw/common/algos/MarchingCubes.h"

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Systems/SandColliders.h"

enum class PlayerAttack {
	SLASH,
	UP_SLASH,
	DOWN_GROUND_SLASH,
	DOWN_AIR_SLASH,
	
	POKE,

	LENGTH
};

struct Player { 
	bool smashAttack = false;
	bool onGround = false;
	bool inAir = false;
	float facing = 1;

	bool k_jump, k_up, k_down, k_left, k_right, k_attack1, k_attack2;

	int hitTick = 0;
	float hitTime = 0;

	int comboHit = 1;
	PlayerAttack lastHit = PlayerAttack::LENGTH; // default that isnt another value

	bool k_charging;
	float charge;

	void AddHit(
		PlayerAttack name,
		int tick)
	{
		if (hitTick == tick) {
			return;
		}

		hitTick = tick;
		hitTime = iw::TotalTime();
		
		if (lastHit == name) {
			comboHit++;
		}

		else {
			lastHit = name;
			comboHit = 2;
		}
	}
};

struct Bullet { float timer = 0; float time = .1; };
struct Grass  {};
struct Enemy  {};
struct Background { float Distance = 1; glm::vec2 Anchor; };

struct GroundSettings {
	float left    = 0, right   = 0;
	float bottom  = 0, top     = 0;
	float xAnchor = 0, yAnchor = 0;
	float xScale  = 5, yScale  = 5;

	bool makeCollider = false;
	bool makeGrass    = false;

	bool isBackground = false;
	bool isWireframe  = false;

	bool makeInSand = false;

	float distance = 1;
	float grassPerMeter = 10;

	float zIndex = 0;

	iw::Color color;
};

struct GroundConstraint : iw::Constraint {
	
	float GroundHeight;
	float Friction;

	GroundConstraint(
		iw::Rigidbody* a,
		float groundHeight = 1,
		float friction = 0
	)
		: iw::Constraint(a, nullptr)
		, GroundHeight(groundHeight)
		, Friction(friction)
	{}

	void init(float dt) override {}

	bool solve(float dt) override {
		float&  y = A->Transform.Position.y;
		float& dy = A->Velocity.y;

		if (y + dy * dt < GroundHeight) {
			y  = GroundHeight;
			dy = 0;

			A->Velocity.x *= 1 - Friction;
		}

		float& x = A->Transform.Position.x;
		float& dx = A->Velocity.x;

		float borderX = 32;

		if (abs(x + dx * dt) > borderX) {
			x = borderX * (signbit(x) ? -1 : 1);
			dx = -dx * .5f;
		}

		return true;
	}
};

struct Attack {
	iw::ref<iw::Space> Space;

	float x, y;
	std::function<bool()> func;

	Attack(
		iw::ref<iw::Space> space,
		float x,
		float y,
		std::function<bool()> func = []() { return false; } // prolly add the entities to this
	)
		: Space(space)
		, x(x)
		, y(y)
		, func(func)
	{}

	void operator()(
		iw::Manifold& manifold,
		iw::scalar dt)
	{
		iw::Entity enemy, hitbox;

		if (iw::GetEntitiesFromManifold<Enemy, Bullet>(Space, manifold, enemy, hitbox)) {
			return;
		}

		iw::Rigidbody*       e = enemy .Find<iw::Rigidbody>();
		iw::CollisionObject* b = hitbox.Find<iw::CollisionObject>();

		e->Velocity.x = x;
		e->Velocity.y = y;

		if (func()) {
			Space->QueueEntity(hitbox.Handle, iw::func_Destroy);
		}
	}
};

struct PixelationLayer
	: iw::Layer
{
	iw::SandLayer* m_sandLayer;
	int m_pixelSize;
	int m_pixelsPerMeter;
	
	iw::OrthographicCamera* cam;
	unsigned lowResWidth;
	unsigned lowResHeight;

	iw::Mesh m_screen;
	iw::Mesh m_square;

	iw::Entity playerEnt;
	iw::Entity tileEntity;

	PixelationLayer(
		iw::SandLayer* sandLayer,
		int pixelSize,
		int pixelsPerMeter
	)
		: iw::Layer("Pixelation")
		, m_sandLayer(sandLayer)
		, m_pixelSize(pixelSize)
		, m_pixelsPerMeter(pixelsPerMeter)
	{}

	int Initialize() {

		iw::ref<iw::Shader> simpleShader = Renderer->InitShader(
			Asset->Load<iw::Shader>("shaders/simple.shader"), 
			iw::CAMERA
		);

		iw::ref<iw::Shader> screenShader = Renderer->InitShader(
			Asset->Load<iw::Shader>("shaders/texture.shader")
		);

		iw::ref<iw::Material> simpleMat = REF<iw::Material>(simpleShader);
		//simpleMat->SetTexture("albedoMap", Asset->Load<iw::Texture>("textures/grass_tuft/baseColor.png"));
		//simpleMat->Set("albedo", iw::Color::From255(78, 186, 84));

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));
		
		m_square = iw::MakePlane(description)->MakeInstance();
		m_square.SetMaterial(simpleMat->MakeInstance());

		// Rotate plane to be facing camera
		{
			iw::Transform rotX;
			rotX.Rotation = glm::angleAxis(-iw::Pi/2, glm::vec3(1, 0, 0));
			m_square.Data()->TransformMeshData(rotX);
		}

		// Player
		if (true) {
			playerEnt = Space->CreateEntity<
				iw::Transform,
				iw::Mesh,
				iw::Hull2,
				iw::Rigidbody,
				iw::Timer,
				Player>();


			iw::Transform*  t = playerEnt.Set<iw::Transform>(glm::vec3(20, 25, 0), glm::vec3(1, 2, 1));
			iw::Mesh*       m = playerEnt.Set<iw::Mesh>(m_square.MakeInstance());
			iw::Hull2*      c = playerEnt.Set<iw::Hull2>(iw::MakeSquareCollider());
			iw::Rigidbody*  r = playerEnt.Set<iw::Rigidbody>();
			Player*         p = playerEnt.Set<Player>();

			iw::Timer* timer = playerEnt.Set<iw::Timer>();
			timer->SetTime("Attack", .25f);
			timer->SetTime("Attack 2", .5f);
			timer->SetTime("Attack 2 Charge", 2.f);

			r->Collider = c;
			r->SetTransform(t);
			r->IsAxisLocked.z = true;

			r->OnCollision = [=](
				iw::Manifold& man, 
				float dt)
			{
				/*iw::Entity e1, e2;
				if(iw::GetEntitiesFromManifold<Player>(Space, man, e1, e2)) {
					return;
				}

				if ()*/

				if (   /*r->Velocity.y == 0
					&& */glm::dot(man.Normal, glm::vec3(0, 1, 0)) > 0)
				{
					p->onGround = true;
				}

				else {
					p->onGround = false;
				}
			};

			Physics->AddRigidbody(r);
			//Physics->AddConstraint(new GroundConstraint(r, -26));

			m->Material()->Set("albedo", iw::Color::From255(240, 100, 100));
		}

		// Enemy
		if (true) {
			iw::Entity enemy = Space->CreateEntity<
				iw::Transform, 
				iw::Mesh,
				iw::Hull2,
				iw::Rigidbody,
				Enemy>();

			iw::Transform* t = enemy.Set<iw::Transform>(glm::vec3(0, 25, 0));
			iw::Mesh*      m = enemy.Set<iw::Mesh>(m_square.MakeInstance());
			iw::Hull2*     c = enemy.Set<iw::Hull2>(iw::MakeSquareCollider());
			iw::Rigidbody* r = enemy.Set<iw::Rigidbody>();
			                   enemy.Set<Enemy>();

			r->Collider = c;
			r->SetTransform(t);
			r->IsAxisLocked.z = true;

			m->Material()->Set("albedo", iw::Color::From255(200, 200, 100));

			Physics->AddRigidbody(r);
			//Physics->AddConstraint(new GroundConstraint(r, -26, .15));
		}

		// Ground
		if (true) {
			GroundSettings mainGround1;
			mainGround1.left = -50;
			mainGround1.right = 50;
			mainGround1.bottom = -10;
			mainGround1.top = 10;
			mainGround1.makeCollider = true;
			//mainGround1.makeGrass = true;
			//mainGround1.grassPerMeter = 10;
			mainGround1.color = iw::Color::From255(138, 84, 37);
			mainGround1.isWireframe = true;
			mainGround1.yAnchor = -20;
			mainGround1.makeInSand = true;

			MakeGroundPlane(mainGround1);

			for (int i = 1; i < 1; i++)
			{
				GroundSettings background;
				background.left = -100 * ceil(sqrt(i));
				background.right = 100 * ceil(sqrt(i));
				background.bottom = -10;
				background.top = 10;
				background.isBackground = true;
				background.distance = i + 1;
				background.zIndex = -i;
				background.color = iw::Color::From255(128, 100 + iw::randf() * 80, 50);
				background.yAnchor = -10;

				MakeGroundPlane(background);
			}
		}

		// Physics

		Physics->SetGravity(glm::vec3(0, -50, 0));
		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());
		iw::SetFixedTime(.01f);

		// Render to low res

		float aspect = float(Renderer->Width()) / Renderer->Height();

		bool lowRes = true;

		lowResWidth  = lowRes ? Renderer->Width()  / m_pixelSize : Renderer->Width();  
		lowResHeight = lowRes ? Renderer->Height() / m_pixelSize : Renderer->Height();

		iw::ref<iw::RenderTarget> lowResTarget = REF<iw::RenderTarget>();
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight));
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight, iw::TEX_2D, iw::DEPTH));

		lowResTarget->Tex(0)->SetFilter(iw::NEAREST);

		// Final screen render - lowres texture onto the screen quad

		iw::ref<iw::Material> screenMat = REF<iw::Material>(screenShader);
		screenMat->SetTexture("texture", lowResTarget->Tex(0));

		m_screen = Renderer->ScreenQuad().MakeInstance();
		m_screen.SetMaterial(screenMat);

		// Rendering and camera

		iw::Color clearColor = iw::Color::From255(0, 37, 53);

		cam = new iw::OrthographicCamera(
			lowResWidth  / m_pixelsPerMeter,
			lowResHeight / m_pixelsPerMeter,
			-1000, 1000
		);

		iw::CameraController* controller = PushSystem<iw::LerpCameraControllerSystem>(playerEnt, cam);
										   PushSystem<iw::RenderSystem>(MainScene, lowResTarget, true, clearColor);
										   PushSystem<iw::PhysicsSystem>();
										   PushSystem<iw::EntityCleanupSystem>();
		
		controller->MakeOrthoOnInit = true;
		controller->Active = false;

		int error = iw::Layer::Initialize();
		if (error) return error;

		//cam = (iw::OrthographicCamera*)controller->GetCamera();

		MainScene->SetMainCamera(cam);

		// Tile

		if (false) {
			tileEntity = m_sandLayer->MakeTile("none", true, true);

			iw::Tile*      tile = tileEntity.Find<iw::Tile>();
			iw::Mesh*      mesh = tileEntity.Find<iw::Mesh>();
			iw::Rigidbody* body = tileEntity.Find<iw::Rigidbody>();

			unsigned* colors = (unsigned*)tile->GetSprite()->Colors();

			for (int y = 1; y < 63; y++)
			for (int x = 48; x < 63; x++)
			{
				colors[x + y * tile->GetSprite()->Width()] = 1;
			}

			mesh->SetMaterial(simpleMat);
			mesh->Material()->Set("albedo", iw::Color::From255(255, 0, 0));
			mesh->Material()->SetWireframe(true);

			body->TakesGravity = false;
			body->Gravity.y = -10;
			body->SetTransform(&iw::Transform(glm::vec3(-20, 10, 01)));

			Physics->AddRigidbody(body);
		}

		// Custom systems

		SandColliderSystem* colliderSystem = PushSystem<SandColliderSystem>(m_sandLayer->m_world, m_pixelsPerMeter);

		colliderSystem->SetCallback([=](iw::Entity entity)
		{
			iw::MeshDescription tileDesc;
			tileDesc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

			iw::Mesh*          mesh     = entity.Set<iw::Mesh>((new iw::MeshData(tileDesc))->MakeInstance());
			iw::MeshCollider2* collider = entity.Find<iw::MeshCollider2>();

			iw::ref<iw::Material> mat = simpleMat;

			mesh->SetMaterial(mat);
			mesh->Material()->Set("albedo", iw::Color::From255(255, 0, 0));
			mesh->Material()->SetWireframe(true);

			mesh->Data()->SetBufferData(iw::bName::POSITION, collider->m_points.size(), collider->m_points.data());
			mesh->Data()->SetIndexData(                      collider->m_index .size(), collider->m_index .data());
		});

		return 0;
	}

	float v[10000];
	float lastHit = 0;

	//float ptimer = 0;
	//int pindex = 0;

	void PostUpdate() {
		//ptimer += iw::DeltaTime();
		//if (ptimer > .1 && pindex < 8) {
		//	ptimer = 0;
		//	pindex += 1;

		//	iw::Cell c;

		//	c.Type = iw::CellType::ROCK;
		//	c.Color = iw::Color(1, 1, 1, 1);

		//	auto drawSqr = [=](int X, int Y) {
		//		for (int x = -8; x < 8; x++)
		//		for (int y = -8; y < 8; y++)
		//		{
		//			if (glm::length(glm::vec2(x, y)) < 4) {
		//				m_sandLayer->m_world->SetCell(x + 16 * X, y + 16 * Y, c);
		//			}
		//		}
		//	};

		//	for (int i = 0; i < 20; i++) {
		//		drawSqr(pindex, iw::randi(20) - 10);
		//	}
		//}

		float camX = cam->WorldPosition().x;
		float camY = cam->WorldPosition().y;
		
		m_sandLayer->SetCamera(camX, camY, 10, 10);

		// Background

		auto P = [](float x) {
			return 1 / iw::clamp<float>(x, 1, 1000);
		};

		Space->Query<iw::Transform, Background>().Each([=](
			auto, 
			iw::Transform* t, 
			Background* b) 
		{
			float scale = P(b->Distance);
			
			t->Position.x = b->Anchor.x + camX * scale;
			t->Position.y = b->Anchor.y + camY * scale;
		});

		// Player

		if (true) {
			Player*        props     = playerEnt.Find<Player>();
			iw::Timer*     player    = playerEnt.Find<iw::Timer>();
			iw::Rigidbody* rigidbody = playerEnt.Find<iw::Rigidbody>();
			iw::Transform& transform = rigidbody->Transform;

			player->Tick();

			int tick = player->CurrentTick;

			if (iw::TotalTime() - props->hitTime > 1) {
				props->comboHit = 1;
				props->hitTick = tick;
				props->hitTime = iw::TotalTime();
				props->lastHit = PlayerAttack::LENGTH;
			}

			float& y = transform.Position.y;
			float xVel = 0;

			props->onGround |= y == -26;
			props->inAir    = !props->onGround/*y > 2*/;

			props->k_jump  = iw::Keyboard::KeyDown(iw::SPACE);
			props->k_up    = iw::Keyboard::KeyDown(iw::W);
			props->k_down  = iw::Keyboard::KeyDown(iw::S);
			props->k_left  = iw::Keyboard::KeyDown(iw::A);
			props->k_right = iw::Keyboard::KeyDown(iw::D);
			props->k_attack1 = iw::Mouse::ButtonDown(iw::LMOUSE);
			props->k_attack2 = iw::Mouse::ButtonDown(iw::RMOUSE);

			if (iw::Keyboard::KeyDown(iw::Y)) {
				props->k_down = true;
				props->k_attack1 = true;
			}

			if (iw::Keyboard::KeyDown(iw::U)) {
				props->k_up = true;
				props->k_attack1 = true;
			}

			if (iw::Keyboard::KeyDown(iw::I)) {
				props->k_attack1 = true;
			}

			// Movement

			if (props->k_left)  xVel -= 10;
			if (props->k_right) xVel += 10;

			if (props->onGround)
			{
				if (props->k_jump) {
					props->onGround = false;
					rigidbody->Velocity.y = 40;
				}

				if (xVel == 0) rigidbody->Velocity.x *= .9;
				else           rigidbody->Velocity.x = xVel;
			}
			else
			{
				if (xVel != 0) rigidbody->Velocity.x = xVel; // Cant stop in air?
			}

			if (abs(xVel) > 0) {
				props->facing = signbit(xVel) ? -1 : 1;
			}

			// Toggle attack flags

			//if (props->k_down) {
			//	if (   props->inAir
			//		&& rigidbody->Velocity.y < .1f) // Start smash attack
			//	{
			//		rigidbody->Velocity.y -= 20 * 20 * iw::DeltaTime();
			//		props->smashAttack = true;
			//	}
			//}

			//else if (props->smashAttack) // Cancel smash attack
			//{
			//	props->smashAttack = false;
			//}

			// Attacks

			if (props->k_attack1 && player->Can("Attack"))
			{
				if (props->k_down)
				{
					if (props->inAir) // Slash down, pushes enemy down but not you, keeps you in air?
					{
						//MakeComboAttack(transform, 
						//	0, -3.5,
						//	1.5, 0.5,
						//	0, -5,
						//	0, -30,
						//	props, tick,
						//	PlayerAttack::DOWN_AIR_SLASH, 2, [=](float xDmg, float yDmg)
						//{
						//	if (props->inAir)
						//	{
						//		rigidbody->Velocity.x = 0;
						//		rigidbody->Velocity.y = -yDmg;
						//	}

						//	return false;
						//});

						MakeAttack(transform,
							2.5f * props->facing, -1, 0.5, -1.5, 
							10   * props->facing, -20, [=]()
						{
							props->AddHit(PlayerAttack::DOWN_AIR_SLASH, tick);
							return false;
						});
					}

					else
					if (props->onGround) // Slash up, pushes enemy in the air but not you
					{
						MakeAttack(transform,
							2.5f * props->facing, 0, 0.5, 1.5,
							2.0f * props->facing, 35, [=]()
						{
							props->AddHit(PlayerAttack::DOWN_GROUND_SLASH, tick);
							return false;
						});
					}
				}

				else
				if (props->k_up) // Sweep above
				{
					MakeComboAttack(transform, 
						0, 2.5, 2.5, 0.5,
						0, 5,
						props->facing * 5, 15,
						props, tick,
						PlayerAttack::UP_SLASH, 2, [=](float xDmg, float yDmg)
					{
						if (props->inAir)
						{
							rigidbody->Velocity.y = yDmg - 1;
						}

						return false;
					});
				}

				//else
				//if (props->k_left || props->k_right)
				//{
				//	MakeAttack(transform,
				//		2.5f * props->facing, 0,
				//		0.5f, 1.5,
				//		15 * props->facing, -15);
				//}

				else // Normal slash forward
				{
					MakeComboAttack(transform,
						2.5f * props->facing, 0, 0.5, 1.5,
						1    * props->facing, 5,
						10   * props->facing, 25,
						props, tick,
						PlayerAttack::SLASH, 3, [=](float xDmg, float yDmg)
					{
						if (props->inAir)
						{
							rigidbody->Velocity.y = yDmg - 1;
						}

						return false;
					});
				}
			}

			else
			if (props->k_attack2 && player->Can("Attack 2"))
			{
				props->k_charging = true;
			}

			if (props->k_charging) {/*
				rigidbody->Velocity.x *= .9;
				rigidbody->Velocity.y *= .9;*/

				props->charge = iw::clamp(props->charge + iw::DeltaTime() * 5, 1.f, 5.f);
				
				if (!props->k_attack2)
				{

					if (props->k_down)
					{
						if (props->inAir) // Slash down, pushes enemy down but not you, keeps you in air?
						{
							//MakeAttack(transform,
							//	2.5f * props->facing, - 1,
							//	0.5f , - 1.5, 
							//	0,                    -40, [=]()
							//{
							//	props->AddHit(PlayerAttack::DOWN_AIR_SLASH, tick);
							//	return false;
							//});
						}

						else
						if (props->onGround) // Slash up, pushes enemy in the air but not you
						{
							//MakeAttack(transform,
							//	2.5f * props->facing,  0,
							//	0.5f ,  1.5,
							//	2.0f * props->facing, 35, [=]()
							//{
							//	props->AddHit(PlayerAttack::DOWN_GROUND_SLASH, tick);
							//	return false;
							//});
						}
					}

					else
					{
						MakeAttack(transform,
							4  * props->facing, 0, 2, 0.25,
							15 * props->charge * props->facing, 0);
					}

					props->k_charging = false;
					props->charge = 1;
				}
			}


			// If on ground
			//
			//	   Down and attack 2 - launch up without moving
			//     Up   and attack 2 - launch up and jump
			//  
			// If in air
			// 
			//     Down and attack 1 - slash down
			//     Down and attack 2 - smash down
			//
			// Regardless
			//
			//    Attack 1 - slash forward
			//    Attack 2 - Poke forward
			//
		}

		// Bullets

		{
			Space->Query<Bullet>().Each([&](
				iw::EntityHandle e,
				Bullet* bullet)
			{
				if ((bullet->timer += iw::DeltaTime()) > bullet->time) {
					Space->QueueEntity(e, iw::func_Destroy);
				}
			});
		}

		// Grass

		if (iw::Keyboard::KeyDown(iw::P)) {
			for (float& vv : v) {
				vv -= iw::DeltaTime() / 4;
			}
		}

		int vi = 0;

		Space->Query<iw::Mesh, Grass>().Each([&](
			iw::EntityHandle,
			iw::Mesh* mesh,
			Grass*)
		{
			iw::ref<iw::MeshData> data = mesh->Data();

			glm::vec3* p = (glm::vec3*)data->Get(iw::bName::POSITION);

			for (size_t i = 1; i < data->GetCount(iw::bName::POSITION); i++, vi++)
			{
				float pa = iw::Pi / 2;

				if (i >= 2) {
					glm::vec3 pp = p[i - 1] - p[i - 2];
					pa = atan2(pp.y, pp.x);
				}

				glm::vec3 p1 = p[i] - p[i - 1];
			
				float a1 = atan2(p1.y, p1.x) + v[vi];

				if (a1 > pa + iw::Pi / 2 - iw::Pi / 6) v[vi] = -iw::DeltaTime() * (iw::randf() + 1);
				if (a1 < pa - iw::Pi / 2 + iw::Pi / 6) v[vi] =  iw::DeltaTime() * (iw::randf() + 1);

				p[i].x = cos(a1) + p[i - 1].x;
				p[i].y = sin(a1) + p[i - 1].y;
			}

			data->Update(Renderer->Device);
		});

		// smooth scrolling, doesnt work like this tho?
		float xd = ((int)camX - camX) / m_pixelSize / m_pixelsPerMeter / m_pixelsPerMeter;
		float yd = ((int)camY - camY) / m_pixelSize / m_pixelsPerMeter / m_pixelsPerMeter;

		Renderer->BeginScene();
			//Renderer->DrawMesh(iw::Transform(), m_sandLayer->GetSandMesh());
			Renderer->DrawMesh(iw::Transform(glm::vec3(xd, yd, 0)), m_screen);
		Renderer->EndScene();
	}

	void MakeAttack(
		iw::Transform& who, 
		float xOff,  float yOff, 
		float xSize, float ySize, 
		float xDmg,  float yDmg,
		std::function<bool()> func = []() { return false; })
	{
		iw::Entity bullet = Space->CreateEntity<
			iw::Transform, 
			iw::CollisionObject, 
			iw::Hull2,
			iw::Mesh, 
			Bullet>();

		iw::Transform*       t = bullet.Set<iw::Transform>();
		iw::CollisionObject* o = bullet.Set<iw::CollisionObject>();
		iw::Hull2*           c = bullet.Set<iw::Hull2>(iw::MakeSquareCollider());
		iw::Mesh*            m = bullet.Set<iw::Mesh>(m_square.MakeInstance());
								 bullet.Set<Bullet>();

		t->Scale.x = xSize;
		t->Scale.y = ySize;

		t->Position.x = who.WorldPosition().x + xOff;
		t->Position.y = who.WorldPosition().y + yOff;
		t->Position.z = 1;

		m->Material()->Set("albedo", iw::Color::From255(200, 200, 255));

		o->Collider = c;
		o->IsTrigger = true;
		o->OnCollision = Attack(Space, xDmg, yDmg, func);
		o->SetTransform(t);

		Physics->AddCollisionObject(o);
	}

	void MakeComboAttack(
		iw::Transform& who, 
		float xOff,      float yOff, 
		float xSize,     float ySize, 
		float xDmg,      float yDmg,
		float xDmgCombo, float yDmgCombo,
		Player* props,     int tick,
		PlayerAttack name, int comboHits,
		std::function<bool(float, float)> func = [](float, float) { return false; })
	{
		float& dmgX = xDmg;
		float& dmgY = yDmg;

		if (   props->lastHit  == name
			&& props->comboHit == comboHits)
		{
			dmgX = xDmgCombo;
			dmgY = yDmgCombo;
		}

		MakeAttack(who, xOff, yOff, xSize, ySize, dmgX, dmgY, [=]()
		{
			props->AddHit(name, tick);
			return func(dmgX, dmgY);
		});
	}

	void MakeGroundPlane(
		const GroundSettings& settings)
	{
		glm::vec2 b1(settings.left,  settings.bottom);
		glm::vec2 b2(settings.right, settings.bottom);

		glm::vec2 t1(settings.left,  settings.top);
		glm::vec2 t2(settings.right, settings.top);

		std::vector<glm::vec2> points { b1 };

		for (float x = settings.left; x < settings.right; x += 20) {
			points.emplace_back(x, settings.bottom);
		}

		points.push_back(b2);
		points.push_back(t2);

		glm::vec2 point = t2;
		while (true) {
			point.x -= (iw::randf() + 1) * settings.xScale;
			point.y +=  iw::randf()      * settings.yScale;

			point.y = iw::clamp(point.y, settings.bottom, FLT_MAX);

			if (point.x <= t1.x) break;

			points.push_back(point);
		}

		points.push_back(t1);

		std::vector<unsigned> index = iw::common::TriangulatePolygon(points);

		if (settings.makeInSand) {
			m_sandLayer->FillPolygon(points, index);
			return;
		}

		std::vector<glm::vec3> points3; 
		for (glm::vec2& v : points) points3.push_back(glm::vec3(v.x, v.y, 0));
		
		iw::ref<iw::Material> simpleMat = REF<iw::Material>(Asset->Load<iw::Shader>("shaders/simple.shader"));

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));

		iw::MeshData* data = new iw::MeshData(description);
		data->SetBufferData(iw::bName::POSITION, points3.size(), points3.data());
		data->SetBufferData(iw::bName::UV, points.size(), points.data());
		data->SetIndexData(index.size(), index.data());

		iw::ref<iw::Material> mat = simpleMat->MakeInstance();
		mat->Set("albedo", settings.color);
		mat->SetWireframe(settings.isWireframe);

		iw::ref<iw::Archetype> arch = Space->CreateArchetype<iw::Transform, iw::Mesh>();

		if (settings.makeCollider) {
			Space->AddComponent<iw::MeshCollider2>  (arch);
			Space->AddComponent<iw::CollisionObject>(arch);
		}

		if (settings.isBackground) {
			Space->AddComponent<Background>(arch);
		}

		iw::Entity ground = Space->CreateEntity(arch);

		iw::Transform*       t = ground.Set<iw::Transform>(glm::vec3(settings.xAnchor, settings.yAnchor, settings.zIndex), glm::vec3(1, 1, 1));
		iw::Mesh*            m = ground.Set<iw::Mesh>(data->MakeInstance());
		
		if (settings.makeCollider) {
			iw::MeshCollider2*   c = ground.Set<iw::MeshCollider2>();
			iw::CollisionObject* o = ground.Set<iw::CollisionObject>();

			c->SetPoints(points);
			c->SetTriangles(index);

			o->Collider = c;
			o->SetTransform(t);

			Physics->AddCollisionObject(o);
		}

		if (settings.isBackground) {
			ground.Set<Background>(settings.distance, glm::vec2(settings.xAnchor, settings.yAnchor));
		}

		m->SetMaterial(mat);

		// Grass
		if (settings.makeGrass) {
			for (size_t i = 0; i < points.size(); i++)
			{
				size_t j = (i + 1) % points.size();

				glm::vec2 norm = points[j] - points[i];

				float temp = norm.x; // get normal
				norm.x = -norm.y;
				norm.y = temp;

				if (glm::dot(norm, glm::vec2(0, 1)) >= 0) {
					continue;
				}

				iw::Mesh lineMesh = iw::MakeLine(description, 3)->MakeInstance();

				size_t count = glm::length(norm) * settings.grassPerMeter;

				for (int s = 0; s < count; s++) {
					iw::Entity grass = Space->CreateEntity<
						iw::Transform,
						iw::Mesh,
						Grass>();

					glm::vec2 pos = iw::lerp(points[i], points[j], float(s) / count);

 					iw::Transform* gt = grass.Set<iw::Transform>(glm::vec3(pos.x, pos.y, 0), glm::vec3(.25));
					iw::Mesh*      gm = grass.Set<iw::Mesh>(lineMesh.MakeCopy());

					gt->SetParent(t);
					gt->Scale = gt->Scale / t->Scale; // cancel scale of t

					iw::ref<iw::Material> mat = simpleMat->MakeInstance();
					mat->Set("albedo", iw::Color::From255(78, 186, 84).rgba() * (1 + iw::randf() * .25f));

					gm->SetMaterial(mat);
				}
			}
		}
	}
};

struct App
	: iw::Application
{
	App() {}

	int Initialize(
		iw::InitOptions& options)
	{
		iw::ref<iw::Context> main = Input->CreateContext("main");
		
		main->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
		main->AddDevice(Input->CreateDevice<iw::RawMouse>());
		main->AddDevice(Input->CreateDevice<iw::Mouse>());

		main->MapButton(iw::T, "toolbox");
		main->MapButton(iw::I, "imgui");

		int pixelScale       = 2;
		int pixelsPerMeter = 10;

		iw::SandLayer* sandLayer = PushLayer<iw::SandLayer>(pixelScale, pixelsPerMeter, true);
		
		int error = iw::Application::Initialize(options);
		if (error) return error;

		PixelationLayer* gameLayer = PushLayer<PixelationLayer>(sandLayer, pixelScale, pixelsPerMeter);
		gameLayer->Initialize();

		return 0;
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions.Width  = 1280;
	options.WindowOptions.Height = 640;
	return new App();
}


iw::Application* GetApplicationForEditor() {
	return new App();
}
