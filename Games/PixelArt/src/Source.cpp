#include "iw/engine/EntryPoint.h"

#include "iw/engine/Systems/Render/RenderSystem.h"
#include "iw/engine/Systems/EditorCameraControllerSystem.h"

#include "iw/physics/Collision/MeshCollider.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"

#include "iw/physics/Dynamics/Mechanism.h"

#include "iw/engine/Components/Timer.h"

#include "iw/common/algos/MarchingCubes.h"

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

	int hitTick = 0; // hack to stop repeat count
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
		float&  y = A->Trans().Position.y;
		float& dy = A->Velocity.y;

		if (y + dy * dt < GroundHeight) {
			y  = GroundHeight;
			dy = 0;

			A->Velocity.x *= 1 - Friction;
		}

		float& x = A->Trans().Position.x;
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
	iw::Mesh m_screen;
	iw::Entity playerEnt;

	PixelationLayer() : iw::Layer("Pixelation") {}

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

		iw::Mesh simpleMesh = iw::MakeLine(description, 3)->MakeInstance();
		iw::Mesh cubeMesh   = iw::MakeCube(description)   ->MakeInstance();

		// Player
		{
			playerEnt = Space->CreateEntity<
				iw::Transform,
				iw::Mesh,
				iw::MeshCollider,
				iw::Rigidbody,
				iw::Timer,
				Player>();

			iw::ref<iw::Material> mat = simpleMat->MakeInstance();
			mat->Set("albedo", iw::Color::From255(240, 100, 100));

			iw::Transform*    t = playerEnt.Set<iw::Transform>(glm::vec3(0, 5, 0), glm::vec3(1));
			iw::Mesh*         m = playerEnt.Set<iw::Mesh>(cubeMesh);
			iw::MeshCollider* c = playerEnt.Set<iw::MeshCollider>(iw::MeshCollider::MakeCube());
			iw::Rigidbody*    r = playerEnt.Set<iw::Rigidbody>();
			Player*           p = playerEnt.Set<Player>();

			iw::Timer* timer = playerEnt.Set<iw::Timer>();
			timer->SetTime("Attack", .25f);
			timer->SetTime("Attack 2", .5f);
			timer->SetTime("Attack 2 Charge", 2.f);

			r->SetCol(c);
			r->SetTrans(t);
			r->IsAxisLocked.z = true;

			r->SetOnCollision([=](
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
			});

			Physics->AddRigidbody(r);
			Physics->AddConstraint(new GroundConstraint(r));

			m->SetMaterial(mat);
		}

		// Enemy
		{
			iw::Entity enemy = Space->CreateEntity<
				iw::Transform, 
				iw::Mesh,
				iw::MeshCollider,
				iw::Rigidbody,
				Enemy>();

			iw::Transform*    t = enemy.Set<iw::Transform>(glm::vec3(0, 5, 0));
			iw::Mesh*         m = enemy.Set<iw::Mesh>(cubeMesh);
			iw::MeshCollider* c = enemy.Set<iw::MeshCollider>(iw::MeshCollider::MakeCube());
			iw::Rigidbody*    r = enemy.Set<iw::Rigidbody>();
			                      enemy.Set<Enemy>();

			r->SetTrans(t);
			r->SetCol(c);
			r->IsAxisLocked.z = true;

			iw::ref<iw::Material> mat = simpleMat->MakeInstance();
			mat->Set("albedo", iw::Color::From255(200, 200, 100));

			m->SetMaterial(mat);

			Physics->AddRigidbody(r);
			Physics->AddConstraint(new GroundConstraint(r, 1, .15));
		}

		// Ground
		{
			iw::Entity ground = Space->CreateEntity<
				iw::Transform,
				iw::Mesh,
				iw::MeshCollider,
				iw::CollisionObject>();

			iw::Transform*       t = ground.Set<iw::Transform>(glm::vec3(0, 5, 0), glm::vec3(5, 1, 1));
			iw::MeshCollider*    c = ground.Set<iw::MeshCollider>();
			iw::CollisionObject* o = ground.Set<iw::CollisionObject>();

			o->SetTrans(t);
			o->SetCol(c);

			Physics->AddCollisionObject(o);

			std::vector<glm::vec2> points {
				glm::vec2(-1,  1),
				glm::vec2(-2, -2),
				glm::vec2( 2, -2),
				glm::vec2( 2,  0.5),
				glm::vec2( 1,  1.2),
				glm::vec2( 0,  1.5),
			};

			std::vector<unsigned> index = iw::common::TriangulatePolygon(points);

			std::vector<glm::vec3> points3; // Translate for 3d for mesh (a hack), also add points to collider
			for (glm::vec2& v : points)
			{
				glm::vec3 v3(v.x, v.y, 0);

				points3.push_back(v3);
				c->AddPoint(v3);
			}

			iw::MeshData* data = new iw::MeshData(description);
			data->SetBufferData(iw::bName::POSITION, points3.size(), points3.data());
			data->SetBufferData(iw::bName::UV, points.size(), points.data());
			data->SetIndexData(index.size(), index.data());

			iw::ref<iw::Material> mat = simpleMat->MakeInstance();
			mat->Set("albedo", iw::Color::From255(138, 84, 37));

			iw::Mesh* m = ground.Set<iw::Mesh>(data->MakeInstance());
			m->SetMaterial(mat);


			// Grass
			{
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

					float step = glm::length(norm) / 50;

					for (int s = 0; s < 50; s++) {
						iw::Entity grass = Space->CreateEntity<
							iw::Transform,
							iw::Mesh,
							Grass>();

						glm::vec2 pos = iw::lerp(points[i], points[j], step * s);

 						iw::Transform* gt = grass.Set<iw::Transform>(glm::vec3(pos.x, pos.y, 0), glm::vec3(.25));
						iw::Mesh*      gm = grass.Set<iw::Mesh>(simpleMesh.MakeCopy());

						gt->SetParent(t);
						gt->Scale = gt->Scale / t->Scale; // cancel scale of t

						iw::ref<iw::Material> mat = simpleMat->MakeInstance();
						mat->Set("albedo", iw::Color::From255(78, 186, 84).rgba() * (1 + iw::randf() * .25f));

						gm->SetMaterial(mat);
					}
				}

				//for (int i = 0; i < 50; i++) {
				//	iw::Entity grass = Space->CreateEntity<
				//		iw::Transform,
				//		iw::Mesh,
				//		Grass>();
				//
				//					 grass.Set<iw::Transform>(glm::vec3(i/5.f - 25, 0, -2), glm::vec3(.25));
				//	iw::Mesh* mesh = grass.Set<iw::Mesh>(simpleMesh.MakeCopy());
				//
				//	iw::ref<iw::Material> mat = simpleMat->MakeInstance();
				//	mat->Set("albedo", iw::Color::From255(78, 186, 84).rgba() * (1 + iw::randf() * .25f));
				//
				//	mesh->SetMaterial(mat);
				//}
			}
		}

		// Physics

		Physics->SetGravity(glm::vec3(0, -100, 0));
		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		// Render to low res

		bool lowRes = true;

		float aspect = float(Renderer->Width()) / Renderer->Height();

		unsigned lowResWidth  = lowRes ? aspect * 256 : Renderer->Width();
		unsigned lowResHeight = lowRes ?          256 : Renderer->Height();

		iw::ref<iw::RenderTarget> lowResTarget = REF<iw::RenderTarget>();
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight));
		lowResTarget->AddTexture(REF<iw::Texture>(lowResWidth, lowResHeight, iw::TEX_2D, iw::DEPTH));

		lowResTarget->Tex(0)->SetFilter(iw::NEAREST);

		// Final screen render that texture onto the screen quad

		iw::ref<iw::Material> screenMat = REF<iw::Material>(screenShader);
		screenMat->SetTexture("texture", lowResTarget->Tex(0));

		m_screen = Renderer->ScreenQuad();
		m_screen.SetMaterial(screenMat);

		// Rendering and camera

		iw::CameraController* controller = PushSystem<iw::EditorCameraControllerSystem>();
										   PushSystem<iw::RenderSystem>(MainScene, lowResTarget, true);
										   PushSystem<iw::PhysicsSystem>();
										   PushSystem<iw::EntityCleanupSystem>();

		controller->MakeOrthoOnInit = true;
		controller->Active = false;

		int error = iw::Layer::Initialize();
		if (error) return error;

		MainScene->SetMainCamera(controller->GetCamera());

		//iw::SetFixedTime(.01f);

		return 0;
	}

	float v[10000];

	void PostUpdate() {

		// Player

		{
			Player*        props     = playerEnt.Find<Player>();
			iw::Timer*     player    = playerEnt.Find<iw::Timer>();
			iw::Rigidbody* rigidbody = playerEnt.Find<iw::Rigidbody>();
			iw::Transform& transform = rigidbody->Trans();

			player->Tick();

			int tick = player->CurrentTick;

			if (tick - props->hitTick > 100) {
				props->comboHit = 1;
				props->hitTick = tick;
				props->lastHit = PlayerAttack::LENGTH;
			}

			float& y = transform.Position.y;
			float xVel = 0;

			props->onGround |= y == 1;
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

			if (props->k_down) {
				if (props->inAir
					&& rigidbody->Velocity.y < .1f) // Start smash attack
				{
					rigidbody->Velocity.y -= 20 * 20 * iw::DeltaTime();
					props->smashAttack = true;
				}
			}

			else if (props->smashAttack) // Cancel smash attack
			{
				props->smashAttack = false;
			}

			// Attacks

			if (props->k_attack1 && player->Can("Attack"))
			{
				if (props->k_down)
				{
					if (props->inAir) // Slash down, pushes enemy down but not you, keeps you in air?
					{
						MakeAttack(transform,
							2.5f * props->facing, - 1,
							0.5f * props->facing, - 1.5, 
							0,                    -40, [=]()
						{
							props->AddHit(PlayerAttack::DOWN_AIR_SLASH, tick);
							return false;
						});
					}

					else
					if (props->onGround) // Slash up, pushes enemy in the air but not you
					{
						MakeAttack(transform,
							2.5f * props->facing,  0,
							0.5f * props->facing,  1.5,
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
						0, 2.5f,
						2.5f, 0.5f,
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

				else // Normal slash forward
				{
					MakeComboAttack(transform,
						2.5f * props->facing, 0,
						0.5f * props->facing, 1.5,
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

			if (props->k_charging) {
				props->charge = iw::clamp(props->charge + iw::DeltaTime() * 5, 1.f, 5.f);
				
				if (!props->k_attack2)
				{

					if (props->k_down)
					{
						if (props->inAir) // Slash down, pushes enemy down but not you, keeps you in air?
						{
							//MakeAttack(transform,
							//	2.5f * props->facing, - 1,
							//	0.5f * props->facing, - 1.5, 
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
							//	0.5f * props->facing,  1.5,
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
							4  * props->facing,                 0,
							2  * props->facing,                 0.25f,
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

		Renderer->BeginScene();
		Renderer->	DrawMesh(iw::Transform(), m_screen);
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
			iw::MeshCollider,
			iw::Mesh, 
			Bullet>();

		iw::Transform*       t = bullet.Set<iw::Transform>(who);
		iw::CollisionObject* o = bullet.Set<iw::CollisionObject>();
		iw::MeshCollider*    c = bullet.Set<iw::MeshCollider>(iw::MeshCollider::MakeCube());
		iw::Mesh*            m = bullet.Set<iw::Mesh>(playerEnt.Find<iw::Mesh>()->MakeInstance());
								    bullet.Set<Bullet>();

		t->Scale.x = xSize;
		t->Scale.y = ySize;

		t->Position.x += xOff;
		t->Position.y += yOff;

		m->Material()->Set("albedo", iw::Color::From255(200, 200, 255));

		o->SetCol(c);
		o->SetTrans(t);
		o->SetOnCollision(Attack(Space, xDmg, yDmg, func));
		o->SetIsTrigger(true);

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
};

struct App
	: iw::Application
{
	App() {
		PushLayer<PixelationLayer>();
	}

	int Initialize(
		iw::InitOptions& options)
	{
		iw::ref<iw::Context> main = Input->CreateContext("main");
		
		main->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
		main->AddDevice(Input->CreateDevice<iw::RawMouse>());
		main->AddDevice(Input->CreateDevice<iw::Mouse>());

		main->MapButton(iw::T, "toolbox");
		main->MapButton(iw::I, "imgui");

		return iw::Application::Initialize(options);
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions.Width  = 1280;
	options.WindowOptions.Height = 720 + 30;
	return new App();
}


iw::Application* GetApplicationForEditor() {
	return new App();
}
