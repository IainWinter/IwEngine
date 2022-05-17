#include "../Entry.h"
#include "../Entity.h"
#include "../Rendering.h"
#include "../Physics.h"
#include "../ext/Time.h"
#include "../ext/marching_cubes.h"
#include "../ext/Sand.h"

#include <iostream>

// temp globals

entity CreatePhysicsEntity(PhysicsWorld& world, const Transform2D& transform = {})
{
	entity e = entities().create();
	e.add<Transform2D>(transform)
	 .add<Rigidbody2D>(transform);

	world.Add(e.get<Rigidbody2D>());
	return e;
}

entity CreateTexturedBox(PhysicsWorld& world, const std::string& path, const std::string& collider_mask_path, const Transform2D& transform = {})
{
	entity entity = CreatePhysicsEntity(world, transform)
		.add<Texture>(path)
		.add<Mesh>();

	Texture collider_mask = Texture(collider_mask_path);
	
	auto polygons = MakePolygonFromField<u32>(
		(u32*)collider_mask.m_host->pixels, 
		collider_mask.Width(), 
		collider_mask.Height(),
		[](const u32& color) { return (color & ~0xff000000) == 0; }
	);

	Rigidbody2D& body = entity.get<Rigidbody2D>();
	//Mesh& mesh = entity.get<Mesh>();

	vec2 scale = vec2(transform.sx, transform.sy);

	for (const std::vector<glm::vec2>& polygon : polygons.first)
	{
		b2PolygonShape shape;

		for (int i = 0; i < polygon.size(); i++)
		{
			//mesh.m_host.push_back(polygon.at(i));
			shape.m_vertices[i] = b2Vec2(polygon.at(i).x * scale.x, polygon.at(i).y * scale.y);
		}

		shape.Set(shape.m_vertices, polygon.size());
		body.m_instance->CreateFixture(&shape, 1.f);
	}

	return entity;
}

entity CreateTexturedCircle(PhysicsWorld& world, const std::string& path, const Transform2D& transform = {})
{
	entity entity = CreatePhysicsEntity(world, transform)
		.add<Texture>(path)
		.add<Mesh>();

	Rigidbody2D& body = entity.get<Rigidbody2D>();

	b2CircleShape shape;
	shape.m_radius = std::max(transform.sx, transform.sy);
	body.m_instance->CreateFixture(&shape, 1.f);

	return entity;
}

// I wonder if you could just declspec a templated type to
// test if it contained these functions and store their func pointers if
// they do
// system shsould never inherit so this should work
// the last engine got bogged down with excessive empty virtual functions

struct PhysicsInterpolationSystem : System
{
	float m_acc = 0.f;

	void Update() override
	{
		m_acc += Time::DeltaTime();
		float ratio = clamp(m_acc / Time::RawFixedTime(), 0.f, 1.f);

		for (auto [e, transform, body] : entities().query<Transform2D, Rigidbody2D>())
		{
			if (!body.InWorld()) continue;

			transform.x = lerp(body.LastTransform.x, body.m_instance->GetPosition().x, ratio);
			transform.y = lerp(body.LastTransform.y, body.m_instance->GetPosition().y, ratio);
			transform.r = lerp(body.LastTransform.r, body.m_instance->GetAngle(),      ratio);
		}
	}

	void FixedUpdate() override
	{
		m_acc = 0;

		for (auto [e, transform, body] : entities().query<Transform2D, Rigidbody2D>())
		{
			if (!body.InWorld()) continue;

			body.LastTransform.x = body.m_instance->GetPosition().x;
			body.LastTransform.y = body.m_instance->GetPosition().y;
			body.LastTransform.r = body.m_instance->GetAngle();
		}
	}
};

struct ForceTwoardwsMouseSystem : System
{
	void FixedUpdate() override
	{
		const Uint8* keystate = SDL_GetKeyboardState(nullptr);

		for (auto [e, transform, body] : entities().query<Transform2D, Rigidbody2D>())
		{
			if (!body.InWorld()) continue;

			float fx = 0 - transform.x;
			float fy = 0 - transform.y;

			body.m_instance->ApplyForceToCenter(b2Vec2(fx, fy), true);
		}
	}
};

struct SpriteRenderer2DSystem : System
{
	void Update() override
	{
		auto [camera, renderer] = entities().first<Camera, SpriteRenderer2D>();

		renderer.Begin(camera, true);
		for (auto [e, transform, sprite] : entities().query<Transform2D, Texture>())
		{
			renderer.DrawSprite(transform, sprite);
		}
	}
};

struct TriangleRenderer2DSystem : System
{
	void Update() override
	{
		auto [camera, renderer] = entities().first<Camera, TriangleRenderer2D>();

		renderer.Begin(camera, false);
		for (auto [e, transform, mesh] : entities().query<Transform2D, Mesh>())
		{
			renderer.DrawMesh(transform, mesh);
		}
	}
};

struct AddSandToWorldSystem : System
{
	AddSandToWorldSystem()
	{
		events().attach<event_Mouse>(this);
	}

	~AddSandToWorldSystem()
	{
		events().detach(this);
	}

	void on(event_Mouse& e)
	{
		auto [window, sand] = entities().first<Window, SandWorld>();

		if (e.button_left)
		{
			sand.CreateCell(e.pixel_x, window.m_config.Height - e.pixel_y, Color::rand())
				.add<CellVel>(get_rand(2.f) - 1.f, get_rand(2.f) - 1.f)
				.add<CellTime>(1.f);
		}
	}
};

struct CharacterController : System
{
	float x, y, speed;

	CharacterController()
	{
		x = 0;
		y = 0;
		speed = 10;
		events().attach<event_Input>(this);
	}

	~CharacterController()
	{
		events().detach(this);
	}

	void FixedUpdate() override
	{
		Rigidbody2D& body = entities().first<Rigidbody2D>();
		body.m_instance->ApplyForceToCenter(b2Vec2(x * speed, y * speed), true);
	}

	void ImGui() override
	{
		Rigidbody2D& body = entities().first<Rigidbody2D>();

		ImGui::Begin("#");
		ImGui::SliderFloat2("pos", (float*)&body.m_instance->GetPosition(), -10, 10);
		ImGui::SliderFloat("speed", &speed, 0, 1000);
		ImGui::End();
	}

	void on(event_Input& e)
	{
		if (e.name == InputName::UP)    y += e.state;
		if (e.name == InputName::DOWN)  y -= e.state;
		if (e.name == InputName::RIGHT) x += e.state;
		if (e.name == InputName::LEFT)  x -= e.state;
	}
};

	// b2EdgeShape shape;
	// shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));

// jobs system for component update
// each job has a list of jobs that it depends on, once those are finished it can begin
// you need to know which components get updated for each job tho to edit concurrently.....

// struct Job
// {

// };

// for now maybe just have a list of systems that get updated at the same time :(

// need a way to update some systems before others tho

using Order = void*;

struct Application
{
	entity m_modules;
	std::vector<System*> m_systems;

	float m_fixedTimeStepAcc;
	bool m_running;

	Application()
	{
		m_fixedTimeStepAcc = 0.f;
		m_running = true;

		events().attach<event_Shutdown> (this);

		WindowConfig windowConfig = {
			"Winter Framework Testbed", 1280, 720
		};

		m_modules = entities().create()
			.add<Window>(windowConfig, &events())
			.add<SpriteRenderer2D>()
			.add<TriangleRenderer2D>()
			.add<PhysicsWorld>()
			.add<SandWorld>(640, 360, 32, 18)
			.add<Camera>(0, 0, 32, 18);

		InputMapping& input = m_modules.get<Window>().m_input;

		input.m_keyboard[SDL_SCANCODE_W] = InputName::UP;
		input.m_keyboard[SDL_SCANCODE_S] = InputName::DOWN;
		input.m_keyboard[SDL_SCANCODE_D] = InputName::RIGHT;
		input.m_keyboard[SDL_SCANCODE_A] = InputName::LEFT;

		m_modules.get<SandWorld>().CreateCell(10, 10, Color(255, 250,  50)).add<CellVel>(.1f, 0.f);
		m_modules.get<SandWorld>().CreateCell(20, 10, Color(255,  50,  50)).add<CellVel>(.2f, 0.f);
		m_modules.get<SandWorld>().CreateCell(30, 20, Color(255,  50, 250)).add<CellVel>(.3f, 0.f);
	}

	~Application()
	{
		events().detach(this);
		for (System* system : m_systems)
		{
			delete system;
		}

		m_modules.destroy();
	}

	template<typename _t>
	_t& Get()
	{
		return m_modules.get<_t>();
	}

	template<typename _t, typename... _args>
	_t* AddSystem(_args&&... args) // need to be able to queue in an order
	{
		_t* system = new _t(args...);
		m_systems.push_back(system);
		return system;
	}

	template<typename _t, typename... _args>
	_t* AddSystemAfter(Order after, _args&&... args) // need to be able to queue in an order
	{
		auto itr = m_systems.begin();
		for (; itr != m_systems.end(); ++itr) if (*itr == after) break;
		
		_t* system = new _t(args...);
		m_systems.insert(itr, system);
		return system;
	}

	bool Step(float deltaTime)
	{
		PhysicsWorld& world = m_modules.get<PhysicsWorld>();

		m_fixedTimeStepAcc += deltaTime;
		if (m_fixedTimeStepAcc > Time::FixedTime())
		{
			m_fixedTimeStepAcc = 0;
			for (System* system : m_systems)
			{
				system->FixedUpdate();
			}

			// This needs to be under here for the last transform loop
			// to interpolate correctly

			world.Step(Time::FixedTime());
		}

		Window& window = m_modules.get<Window>();

		for (System* system : m_systems)
		{
			system->Update();
		}

		m_modules.get<SandWorld>().Update(); // remove this and add a system

		window.BeginImgui();
		for (System* system : m_systems)
		{
			system->ImGui();
		}
		window.EndImgui();

		window.EndFrame();
		window.PumpEvents();

		return m_running;
	}

	// these cant be in the ecs because they need an order of execution
	// this is where a jobs system comes in I think, I guess you make a strand
	// and at each level it can be multithreaded?
	// I guess each loop through the entities actually could queue work in a thread pool
	// and then exeacute all 100000 work items

	void on(event_Shutdown& e)
	{
		m_running = false;
	}
};

Application app;

void setup()
{
	app.AddSystem<ForceTwoardwsMouseSystem>();
	app.AddSystem<PhysicsInterpolationSystem>();
	app.AddSystem<SpriteRenderer2DSystem>();
	//app.AddSystem<TriangleRenderer2DSystem>();
	app.AddSystem<CharacterController>();
	app.AddSystem<Sand_LifeUpdateSystem>();
	app.AddSystem<Sand_VelUpdateSystem>();
	app.AddSystem<AddSandToWorldSystem>();

	PhysicsWorld& world = app.Get<PhysicsWorld>();
	
	for (int i = 0; i < 3; i++)
	{
		Transform2D t;
		t.sx = 2;
		t.sy = 2;
		t.x = 10 + i * 10;
		CreateTexturedBox(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_station.png", "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_station_mask.png", t);
	}

	Transform2D t;
	t.sx = 2;
	t.sy = 2;

	CreateTexturedBox(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_base.png", "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_base_mask.png", t);
	
	t.sx = .75;
	t.sy = .75;
	t.x = -10;
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
	CreateTexturedCircle(world, "C:/dev/IwEngine/_assets/textures/SpaceGame/enemy_bomb.png", t);
}

bool loop()
{
	Time::UpdateTime();
	return app.Step(Time::DeltaTime());
}