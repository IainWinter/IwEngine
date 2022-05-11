#include "../Entry.h"
#include "../Entity.h"
#include "../Rendering.h"
#include "../Physics.h"
#include "../ext/Time.h"

#include <iostream>

// temp globals

float mouseX, mouseY;

float get_rand(float x)
{
	return x * rand() / (float)RAND_MAX;
}

int get_rand(int x)
{
	return rand() % x;
}

entity CreatePhysicsEntity(PhysicsWorld& world)
{
	entity e = entities().create<Transform2D, Rigidbody2D>();
	world.Add(e.get<Rigidbody2D>());
	return e;
}

entity CreateTexturedBox(PhysicsWorld& world)
{
	b2PolygonShape shape;
	shape.SetAsBox(1, 1);

	entity entity = CreatePhysicsEntity(world);

	entity.get<Rigidbody2D>().m_instance->CreateFixture(&shape, 1.0f);
	entity.add<Texture>(
		"C:/dev/IwEngine/WinterFramework/test/Untitled.bmp",
		Color(get_rand(255), get_rand(255), get_rand(255))
	);

	return entity;
}

// I wonder if you could just declspec a templated type to
// test if it contained these functions and store their func pointers if
// they do
// system shsould never inherit so this should work
// the last engine got bogged down with excessive empty virtual functions

struct System
{
	virtual void Update() {}
	virtual void FixedUpdate() {}
	virtual void ImGui() {}
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

		renderer.Begin(camera);
		for (auto [transform, sprite] : entities().query<Transform2D, Texture>())
		{
			renderer.DrawSprite(transform, sprite);
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
		speed = 300;
		events().attach<event_Input>(this);
	}

	~CharacterController()
	{
		events().detach(this);
	}

	void FixedUpdate() override
	{
		auto [body] = entities().query<Rigidbody2D>().first();
		body.m_instance->ApplyForceToCenter(b2Vec2(x * speed, y * speed), true);

		b2Vec2 pos = body.m_instance->GetPosition();

		printf("%.2f, %.2f  %1.0f, %1.0f\n", pos.x, pos.y, x, y);
	}

	void ImGui() override
	{
		auto [body] = entities().query<Rigidbody2D>().first();

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
		events().attach<event_MouseMove>(this);

		WindowConfig windowConfig = {
			"Winter Framework Testbed", 1280, 720
		};

		m_modules = entities().create()
			.add<Window>(windowConfig, &events())
			.add<SpriteRenderer2D>()
			.add<PhysicsWorld>()
			.add<Camera>(0, 0, 32, 18);


		InputMapping& input = m_modules.get<Window>().m_input;

		input.m_keyboard[SDL_SCANCODE_W] = InputName::UP;
		input.m_keyboard[SDL_SCANCODE_S] = InputName::DOWN;
		input.m_keyboard[SDL_SCANCODE_D] = InputName::RIGHT;
		input.m_keyboard[SDL_SCANCODE_A] = InputName::LEFT;
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
	void AddSystem(_args&&... args) // need to be able to queue in an order
	{
		m_systems.push_back(new _t(args...));
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

	void on(event_MouseMove& e)
	{
		mouseX = e.x;
		mouseY = e.y;
	}
};

Application app;

void setup()
{
	app.AddSystem<PhysicsInterpolationSystem>();
	app.AddSystem<ForceTwoardwsMouseSystem>();
	app.AddSystem<SpriteRenderer2DSystem>();
	app.AddSystem<CharacterController>();

	PhysicsWorld& world = app.Get<PhysicsWorld>();
	
	for (int i = 0; i < 50; i++)
	{
		CreateTexturedBox(world);
	}
}

bool loop()
{
	Time::UpdateTime();
	return app.Step(Time::DeltaTime());
}