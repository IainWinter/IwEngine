#include "../Entry.h"
#include "../EntityMine.h"
#include "../Rendering.h"
#include "../Windowing.h"
#include "../Physics.h"
#include "../ext/Time.h"
#include "../ext/marching_cubes.h"
#include "../ext/Sand.h"

#include <iostream>

std::string _p(const std::string& filename)
{
	return "C:/dev/IwEngine/_assets/textures/SpaceGame/" + filename;
}

using Order = void*;

struct Application
{
	entity m_modules;
	std::vector<System*> m_systems;

	float m_fixedTimeStepAcc;
	bool m_running;

	Application()
	{
		events().attach<event_Shutdown>(this);
		m_fixedTimeStepAcc = 0.f;
		m_running = true;
		m_modules = entities().create();
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
			system->UI();
		}
		window.EndImgui();

		window.EndFrame();
		window.PumpEvents();

		entities_defer().execute();
		entities().clean_storage();
		
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

Application app; // for this simple test use global

struct Player
{
	float speed = 300;
};

// temp globals

entity CreatePhysicsEntity(const Transform2D& transform = {})
{
	PhysicsWorld& world = app.Get<PhysicsWorld>();

	entity e = entities().create();
	e.add<Transform2D>(transform)
	 .add<Rigidbody2D>(transform);

	world.Add(e.get<Rigidbody2D>());
	return e;
}

entity CreateTexturedBox(const std::string& path, const std::string& collider_mask_path, Transform2D transform = {})
{	
	Texture sprite = Texture(_p(path));
	SandWorld& sand = app.Get<SandWorld>();
	transform.sx = sprite.Width()  / sand.worldScale.x;
	transform.sy = sprite.Height() / sand.worldScale.y;

	entity entity = CreatePhysicsEntity(transform)
		.add<Texture>(sprite);

	Texture collider_mask = Texture(_p(collider_mask_path));
	auto polygons = MakePolygonFromField<u32>(
		(u32*)collider_mask.Pixels(), 
		collider_mask.Width(), 
		collider_mask.Height(),
		[](const u32& color) { return (color & ~0xff000000) == 0; }
	);

	Rigidbody2D& body = entity.get<Rigidbody2D>();
	vec2 scale = vec2(transform.sx, transform.sy);
	for (const std::vector<glm::vec2>& polygon : polygons.first)
	{
		b2PolygonShape shape;
		for (int i = 0; i < polygon.size(); i++)
		{
			shape.m_vertices[i] = b2Vec2(polygon.at(i).x * scale.x, polygon.at(i).y * scale.y);
		}
		shape.Set(shape.m_vertices, polygon.size());
		body.m_instance->CreateFixture(&shape, 1.f);
	}

	return entity;
}

entity CreateTexturedCircle(const std::string& path, Transform2D transform = {})
{
	Texture sprite = Texture(_p(path));
	SandWorld& sand = app.Get<SandWorld>();
	transform.sx = sprite.Width()  / sand.worldScale.x;
	transform.sy = sprite.Height() / sand.worldScale.y;

	entity entity = CreatePhysicsEntity(transform)
		.add<Texture>(sprite);

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

struct EventLoggingSystem : System
{
	EventLoggingSystem()
	{
		events().attach<event_Shutdown>(this);
		events().attach<event_WindowResize>(this);
		events().attach<event_Mouse>(this);
		events().attach<event_Input>(this);
	}

	~EventLoggingSystem()
	{
		events().detach(this);
	}

	void on(event_Shutdown& e)
	{
		printf(
			"[Event] Recieved: event_Shutdown {}\n");
	}

	void on(event_WindowResize& e)
	{
		printf(
			"[Event] Recieved: event_WindowResize {"
			"\n\twidth %d"
			"\n\theight %d"
			"\n}\n", e.width, e.height);
	}

	void on(event_Mouse& e)
	{	
		printf(
			"[Event] Recieved: event_Mouse {"
			"\n\tpixel_x %d"
			"\n\tpixel_y %d"
			"\n\tscreen_x %f"
			"\n\tscreen_y %f"
			"\n\tvel_x %f"
			"\n\tvel_y %f"
			"\n\tbutton_left %d"
			"\n\tbutton_middle %d"
			"\n\tbutton_right %d"
			"\n\tbutton_x1 %d"
			"\n\tbutton_x2 %d"
			"\n\tbutton_repeat %d"
			"\n}\n", e.pixel_x, e.pixel_y, e.screen_x, e.screen_y, 
				     e.vel_x, e.vel_y, e.button_left, e.button_middle,
				     e.button_right, e.button_x1, e.button_x2, e.button_repeat);
	}

	void on(event_Input& e)
	{
		static std::unordered_map<InputName, const char*> names = 
		{
			{ InputName::_NONE, "None" },
			{ InputName::UP,    "Up" },
			{ InputName::DOWN,  "Down" },
			{ InputName::RIGHT, "Right" },
			{ InputName::LEFT,  "Left" },
		};

		printf(
			"[Event] Recieved: event_Input {"
			"\n\tname %d"
			"\n\tstate %f"
			"\n}\n", names.at(e.name), e.state);
	}
};

struct PhysicsInterpolationSystem : System
{
	float m_acc = 0.f;

	void Update() override
	{
		m_acc += Time::DeltaTime();
		float ratio = clamp(m_acc / Time::RawFixedTime(), 0.f, 1.f);

		for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
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

		for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
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

		for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
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
		auto [camera, renderer] = entities().query<Camera, SpriteRenderer2D>().first();

		renderer.Begin(camera, true);
		for (auto [transform, sprite] : entities().query<Transform2D, Texture>())
		{
			renderer.DrawSprite(transform, sprite);
		}
	}
};

struct TriangleRenderer2DSystem : System
{
	void Update() override
	{
		auto [camera, renderer] = entities().query<Camera, TriangleRenderer2D>().first();

		renderer.Begin(camera, false);
		for (auto [transform, mesh] : entities().query<Transform2D, Mesh>())
		{
			renderer.DrawMesh(transform, mesh);
		}
	}
};

struct CharacterController : System
{
	float x = 0;
	float y = 0;
	float vx = 0;
	float vy = 0;
	float speed = 0;
	bool mouseDown = false;

	float mouseX, mouseY; // temp

	CharacterController()
	{
		events().attach<event_Input>(this);
		events().attach<event_Mouse>(this);
	}

	~CharacterController()
	{
		events().detach(this);
	}

	void Update() override
	{
		auto [sand] = entities().query<SandWorld>().first();

		auto [player, transform] = entities().query<Player, Transform2D>().first();

		if (mouseDown)
		{
			sand.CreateCell(transform.x, transform.y, Color(255, 255, 19))
				.add<CellVel>(x * 700 + get_rand(200) - 10, y * 700 + get_rand(020) - 10, 100.f)
				.add<CellLife>(5.f * get_rand(1.f));
		}
		
		//sand.CreateCell(mouseX, mouseY, Color(255, 100, 100));
	}

	void FixedUpdate() override
	{
		auto [body] = entities().query<Rigidbody2D>().first();
		body.m_instance->ApplyForceToCenter(b2Vec2(vx * speed, vy * speed), true);
	}

	void UI() override
	{
		auto [body] = entities().query<Rigidbody2D>().first();

		float ts = Time::TimeScale();

		ImGui::Begin("#");
		ImGui::SliderFloat2("pos", (float*)&body.m_instance->GetPosition(), -10, 10);
		ImGui::SliderFloat("speed", &speed, 0, 1000);
		ImGui::SliderFloat("time", &ts, 0, 2);
		ImGui::Text("Deltatime: %f", Time::DeltaTime());
		ImGui::End();

		if (ts != Time::TimeScale())
		{
			Time::SetTimeScale(ts);
		}

		ImGui::Begin("Archetypes");
		for (auto& [_, storage] : entities().m_storage)
		{
			ImGui::Text("%zd Count: %d", storage.m_archetype.m_hash, storage.m_count);
		}
		ImGui::End();
	}

	void on(event_Input& e)
	{
		if (e.name == InputName::UP)    vy += e.state;
		if (e.name == InputName::DOWN)  vy -= e.state;
		if (e.name == InputName::RIGHT) vx += e.state;
		if (e.name == InputName::LEFT)  vx -= e.state;
	}

	void on(event_Mouse& e)
	{
		mouseDown = e.button_left;
		x = e.screen_x;
		y = e.screen_y;

		float d = sqrt(x*x + y*y);
		x /= d;
		y /= d;

		x *= 2;
		y *= 2;

		mouseX = e.screen_x;
		mouseY = e.screen_y;
	}
};

void setup_modules()
{
	WindowConfig windowConfig = {
		"Winter Framework Testbed", 1280, 720
	};

	app.m_modules
		.add<Window>(windowConfig, &events())
		.add<SpriteRenderer2D>()
		.add<TriangleRenderer2D>()
		.add<PhysicsWorld>()
		.add<SandWorld>(1280, 720, 32, 18)
		.add<Camera>(0, 0, 32, 18);
}

void setup_systems()
{
	app.AddSystem<ForceTwoardwsMouseSystem>();
	app.AddSystem<PhysicsInterpolationSystem>();
	app.AddSystem<SpriteRenderer2DSystem>();
	app.AddSystem<CharacterController>();
	app.AddSystem<Sand_LifeUpdateSystem>();
	app.AddSystem<Sand_VelUpdateSystem>();
	//app.AddSystem<TriangleRenderer2DSystem>();
	//app.AddSystem<AddSandToWorldSystem>();
	//app.AddSystem<EventLoggingSystem>();
}

void setup_inputmapping()
{
	InputMapping& input = app.Get<Window>().m_input;

	input.m_keyboard[SDL_SCANCODE_W] = InputName::UP;
	input.m_keyboard[SDL_SCANCODE_S] = InputName::DOWN;
	input.m_keyboard[SDL_SCANCODE_D] = InputName::RIGHT;
	input.m_keyboard[SDL_SCANCODE_A] = InputName::LEFT;
}

void setup()
{
	setup_modules();
	setup_systems();
	setup_inputmapping();

	for (int i = 0; i < 3; i++)
	{
		Transform2D t;
		t.x = 10 + i * 10;
		CreateTexturedBox("enemy_station.png", "enemy_station_mask.png", t);
	}

	Transform2D t;

	CreateTexturedBox("enemy_base.png", "enemy_base_mask.png", t);
	
	t.x = -10;
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t);
	CreateTexturedCircle("enemy_bomb.png", t).add<Player>();
}

bool loop()
{
	Time::UpdateTime();
	return app.Step(Time::DeltaTime());
}