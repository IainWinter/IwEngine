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
#include "iw/graphics/Model.h"
#include "iw/common/algos/polygon2.h"

#include "plugins/iw/Sand/Engine/SandLayer.h"

#include "Systems/SandColliders.h"
#include "Systems/Player.h"
#include "Systems/Attack.h"
#include "Systems/PlayerAttack.h"

//enum class PlayerAttack {
//	SLASH,
//	UP_SLASH,
//	DOWN_GROUND_SLASH,
//	DOWN_AIR_SLASH,
//	
//	POKE,
//
//	LENGTH
//};

//struct Player { 
//	bool smashAttack = false;
//	bool onGround = false;
//	bool inAir = false;
//	float facing = 1;
//
//	bool k_jump, k_up, k_down, k_left, k_right, k_attack1, k_attack2;
//
//	int hitTick = 0;
//	float hitTime = 0;
//
//	int comboHit = 1;
//	PlayerAttack lastHit = PlayerAttack::LENGTH; // default that isnt another value
//
//	bool k_charging;
//	float charge;
//
//	void AddHit(
//		PlayerAttack name,
//		int tick)
//	{
//		if (hitTick == tick) {
//			return;
//		}
//
//		hitTick = tick;
//		hitTime = iw::TotalTime();
//		
//		if (lastHit == name) {
//			comboHit++;
//		}
//
//		else {
//			lastHit = name;
//			comboHit = 2;
//		}
//	}
//};

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

	std::string mesh;
	bool makeFromMesh = false;
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

//struct Attack {
//	iw::ref<iw::Space> Space;
//
//	float x, y;
//	std::function<bool()> func;
//
//	Attack(
//		iw::ref<iw::Space> space,
//		float x,
//		float y,
//		std::function<bool()> func = []() { return false; } // prolly add the entities to this
//	)
//		: Space(space)
//		, x(x)
//		, y(y)
//		, func(func)
//	{}
//
//	void operator()(
//		iw::Manifold& manifold,
//		iw::scalar dt)
//	{
//		iw::Entity enemy, hitbox;
//
//		if (iw::GetEntitiesFromManifold<Enemy, Bullet>(Space, manifold, enemy, hitbox)) {
//			return;
//		}
//
//		iw::Rigidbody*       e = enemy .Find<iw::Rigidbody>();
//		iw::CollisionObject* b = hitbox.Find<iw::CollisionObject>();
//
//		e->Velocity.x = x;
//		e->Velocity.y = y;
//
//		if (func()) {
//			Space->QueueEntity(hitbox.Handle, iw::func_Destroy);
//		}
//	}
//};

struct PixelationLayer
	: iw::Layer
{
	iw::SandLayer* m_sandLayer;
	int m_pixelSize;
	int m_pixelsPerMeter;
	
	SandColliderSystem* m_colliderSystem;

	iw::OrthographicCamera* cam;
	unsigned lowResWidth;
	unsigned lowResHeight;

	iw::Mesh m_screen;
	iw::Mesh m_square;

	//iw::Entity playerEnt;
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
		//simpleMat->SetTexture("texture", Asset->Load<iw::Texture>("textures/grass_tuft/baseColor.png"));
		//simpleMat->Set("color", iw::Color::From255(78, 186, 84));

		iw::MeshDescription description;
		description.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(3));
		description.DescribeBuffer(iw::bName::UV,       iw::MakeLayout<float>(2));
		
		m_square = iw::MakePlane(description)->MakeInstance();
		m_square.Material = simpleMat->MakeInstance();

		// Rotate plane to be facing camera
		{
			iw::Transform rotX;
			rotX.Rotation = glm::angleAxis(-iw::Pi/2, glm::vec3(1, 0, 0));
			m_square.Data->TransformMeshData(rotX);
		}

		// Enemy
		if (true) {
			iw::Entity enemy = Space->CreateEntity<
				iw::Transform, 
				iw::Mesh,
				iw::Hull2,
				iw::Rigidbody,
				Enemy>();

			iw::Transform* t = enemy.Set<iw::Transform>(glm::vec3(0, 25, 0), glm::vec3(1, 2, 1));
			iw::Mesh*      m = enemy.Set<iw::Mesh>(m_square.MakeInstance());
			iw::Hull2*     c = enemy.Set<iw::Hull2>(iw::MakeSquareCollider());
			iw::Rigidbody* r = enemy.Set<iw::Rigidbody>();
			                   enemy.Set<Enemy>();

			r->Collider = c;
			r->SetTransform(t);
			r->IsAxisLocked.z = true;

			m->Material->Set("color", iw::Color::From255(200, 200, 100));

			Physics->AddRigidbody(r);
			//Physics->AddConstraint(new GroundConstraint(r, -26, .15));
		}

		// Ground
		if (false) {
			GroundSettings mainGround1;
			//mainGround1.makeGrass = true;
			//mainGround1.grassPerMeter = 10;
			mainGround1.makeInSand = true;
			mainGround1.makeFromMesh = true;
			mainGround1.mesh = "levels/Sand/levelTest.glb";

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

		iw::Cell c;
		c.Type = iw::CellType::ROCK;
		c.Color = iw::Color::From255(100, 100, 100);

		for (int y = 0; y < 5; y++)
		for (int x = -200; x < 500; x++)
		{
			m_sandLayer->m_world->SetCell(x, y, c);
		}

		// Physics

		Physics->SetGravity(glm::vec3(0, -80, 0));
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
		m_screen.Material = screenMat;

		// Custom systems
	
		PlayerSystem* playerSystem = PushSystem<PlayerSystem>(m_square);
		                             PushSystem<PlayerAttackSystem>();
		                             PushSystem<AttackSystem>(m_square);

		m_colliderSystem = PushSystem<SandColliderSystem>(m_sandLayer->m_world, m_pixelsPerMeter);

		if (true) // drawing colliders 
		{
			m_colliderSystem->MadeColliderCallback = [=](
				iw::Entity entity) 
			{
				iw::MeshDescription tileDesc;
				tileDesc.DescribeBuffer(iw::bName::POSITION, iw::MakeLayout<float>(2));

				iw::Mesh*          mesh     = entity.Set<iw::Mesh>((new iw::MeshData(tileDesc))->MakeInstance());
				iw::MeshCollider2* collider = entity.Find<iw::MeshCollider2>();

				iw::ref<iw::Material> mat = simpleMat;

				mesh->Material = mat;
				mesh->Material->Set("color", iw::Color::From255(255, 0, 0));
				mesh->Material->SetWireframe(true);

				mesh->Data->SetBufferData(iw::bName::POSITION, collider->m_points.size(), collider->m_points.data());
				mesh->Data->SetIndexData(                      collider->m_index .size(), collider->m_index .data());
			};
		}

		if (true)
		{
			m_colliderSystem->CutColliderCallback = [=](
				iw::MeshCollider2* mesh, 
				iw::polygon_cut& cut) 
			{
				auto& [lverts, lindex, rverts, rindex] = cut;
				if (rindex.size() == 0) return;

				//m_sandLayer->FillPolygon(rverts, rindex, iw::Cell());

				iw::AABB2 bounds = iw::GenPolygonBounds(rverts);
				glm::ivec2 dim = (bounds.Max - bounds.Min) * float(m_pixelsPerMeter);

				iw::ref<iw::Texture> sprite = REF<iw::Texture>(dim.x, dim.y);
				sprite->SetFilter(iw::NEAREST);
				
				unsigned* colors = (unsigned*)sprite->CreateColors();

				int numbTested = 0;
				int numbOutside = 0;

				m_sandLayer->ForEachInPolygon(rverts, rindex, [&](
					float s, float t, 
					int   x, int   y)
				{
					numbTested++;

					int px = x - bounds.Min.x * m_pixelsPerMeter; // local to sprite
					int py = y - bounds.Min.y * m_pixelsPerMeter;

					if (!m_sandLayer->m_world->IsEmpty(x, y))
					{
						if (size_t(px + py * dim.x) < dim.x * dim.y)
						{
							colors[px + py * dim.x] = m_sandLayer->m_world->GetCell(x, y).Color.to32();
						}

						else {
							numbOutside++;
						}

						m_sandLayer->m_world->SetCell(x, y, iw::Cell());
					}
				});

				LOG_INFO << double(numbOutside) / numbTested * 100 << "% of the cells were outside the sprite";

				iw::Entity entity = m_sandLayer->MakeTile(sprite, true);
				entity.Find<iw::Rigidbody>()->Transform.Position = glm::vec3(bounds.Center(), 0);
			};
		}

		// Rendering and camera

		iw::Color clearColor = iw::Color::From255(0, 37, 53);

		cam = new iw::OrthographicCamera(
			lowResWidth  / m_pixelsPerMeter,
			lowResHeight / m_pixelsPerMeter,
			-1000, 1000
		);

		iw::CameraController* controller = PushSystem<iw::LerpCameraControllerSystem>(playerSystem->PlayerEntity, cam);
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
		
		if (false) 
		for (int i = 0; i < 3; i++) {
			tileEntity = m_sandLayer->MakeTile("none", true);

			iw::Tile* tile = tileEntity.Find<iw::Tile>();

			unsigned* colors = (unsigned*)tile->m_sprite->Colors();

			for (int y = 0; y < 64; y++)
			for (int x = 0; x < 16; x++)
			{
				colors[x + y * tile->m_sprite->Width()] = 1;
			}

			tileEntity.Find<iw::Rigidbody>()->Transform.Position = glm::vec3(i*10, 20, 0);
		}

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

		if (iw::Keyboard::KeyDown(iw::H)) {
			m_colliderSystem->CutWorld(glm::vec2(15, -20), glm::vec2(18, 20));
		}

		float camX = cam->WorldPosition().x;
		float camY = cam->WorldPosition().y;
		
		m_sandLayer->SetCamera(camX, camY, m_pixelsPerMeter, m_pixelsPerMeter); // this is 1 frame behind

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
			iw::ref<iw::MeshData> data = mesh->Data;

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
			Renderer->DrawMesh(iw::Transform(/*glm::vec3(xd, yd, 0)*/), m_sandLayer->GetSandMesh());
			Renderer->DrawMesh(iw::Transform(), m_screen);
		Renderer->EndScene();
	}

	void MakeGroundPlane(
		const GroundSettings& settings)
	{
		glm::vec2 b1(settings.left,  settings.bottom);
		glm::vec2 b2(settings.right, settings.bottom);

		glm::vec2 t1(settings.left,  settings.top);
		glm::vec2 t2(settings.right, settings.top);

		std::vector<glm::vec2> points;
		std::vector<unsigned> index;

		if (settings.makeFromMesh) {
			iw::ref<iw::MeshData> mesh = Asset->Load<iw::Model>(settings.mesh)->GetMesh(0).Data;

			glm::vec3* pos = (glm::vec3*)mesh->Get(iw::bName::POSITION);
			unsigned*  idx = mesh->GetIndex();

			for (size_t i = 0; i < mesh->GetCount(iw::bName::POSITION); i++) {
				points.emplace_back(pos[i].x, pos[i].y);
			}

			for (size_t i = 0; i < mesh->GetIndexCount(); i++) {
				index.push_back(idx[i]);
			}
		}

		else {
			points.push_back(b1);

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

			index = iw::TriangulatePolygon(points);
		}

		if (settings.makeInSand) {
			iw::Cell cell;
			cell.Color = iw::Color::From255(100, 100, 100);
			cell.Type = iw::CellType::ROCK;

			m_sandLayer->FillPolygon(points, index, cell);

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
		mat->Set("color", settings.color);
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

		iw::Transform* t = ground.Set<iw::Transform>(glm::vec3(settings.xAnchor, settings.yAnchor, settings.zIndex), glm::vec3(1, 1, 1));
		iw::Mesh*      m = ground.Set<iw::Mesh>(data->MakeInstance());
		
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

		m->Material = mat;

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
					mat->Set("color", iw::Color::From255(78, 186, 84).rgba() * (1 + iw::randf() * .25f));

					gm->Material = mat;
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



//
//collider.AddPoint(glm::vec2(-.9, 1)); // 0
//collider.AddPoint(glm::vec2(-1, .9)); // 1 
//
//collider.AddPoint(glm::vec2(-1, -.9)); // 1 
//collider.AddPoint(glm::vec2(-.9, -1)); // 2
//
//collider.AddPoint(glm::vec2(.9, -1)); // 3
//collider.AddPoint(glm::vec2(1, -.9)); // 4
//
//collider.AddPoint(glm::vec2(1, .9)); // 5 
//collider.AddPoint(glm::vec2(.9, 1)); // 6 
