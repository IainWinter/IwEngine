#include "../Entry.h"
#include "../Entity.h"
#include "../Rendering.h"
#include "../Physics.h"
#include "../ext/Time.h"

#include <iostream>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "glad/glad.h"

// temp globals

float mouseX, mouseY;

float get_rand(float x)
{
	return x * rand() / (float)RAND_MAX;
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
	shape.SetAsBox(60, 60);

	entity entity = CreatePhysicsEntity(world);

	entity.get<Rigidbody2D>().m_instance->CreateFixture(&shape, 1.0f);
	entity.add<Texture>("C:/dev/IwEngine/WinterFramework/test/Untitled.bmp");

	return entity;
}

// I wonder if you could just declspec a templated type to
// test if it contained these functions and store their func pointers if
// they do
// system shsould never inherit so this should work
// the last engine got bogged down with excessive empty virtual functions

struct System
{
	virtual void Update() {};
	virtual void FixedUpdate() {};
};

struct PhysicsInterpolationSystem : System
{
	float m_acc = 0.f;

	void Update() override
	{
		m_acc += Time::DeltaTime();
		float ratio = clamp(m_acc / Time::RawFixedTime(), 0.f, 1.f);

		// for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
		// {
		// 	if (!body.InWorld()) continue;
		// 	printf("%f\n", transform.x);

		// 	transform.x = lerp(body.LastTransform.x, body.m_instance->GetPosition().x, ratio);
		// 	transform.y = lerp(body.LastTransform.y, body.m_instance->GetPosition().y, ratio);
		// 	transform.r = lerp(body.LastTransform.r, body.m_instance->GetAngle(),      ratio);
		// }
	}

	void FixedUpdate() override
	{
		m_acc = 0;

		for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
		{
			if (!body.InWorld()) continue;

			// body.LastTransform.x = body.m_instance->GetPosition().x;
			// body.LastTransform.y = body.m_instance->GetPosition().y;
			// body.LastTransform.r = body.m_instance->GetAngle();

			
			transform.x = body.m_instance->GetPosition().x;
			transform.y = body.m_instance->GetPosition().y;
			transform.r = body.m_instance->GetAngle();
		}
	}
};

struct ForceTwoardwsMouseSystem : System
{
	void FixedUpdate() override
	{
		for (auto [transform, body] : entities().query<Transform2D, Rigidbody2D>())
		{
			if (!body.InWorld()) continue;

			float fx = mouseX - transform.x;
			float fy = mouseY - transform.y;

			body.m_instance->SetLinearVelocity(b2Vec2(10000, 0));
		}
	}
};

	// b2EdgeShape shape;
	// shape.SetTwoSided(b2Vec2(-40.0f, 0.0f), b2Vec2(40.0f, 0.0f));


struct Application
{
	entity modules;
	std::vector<System*> systems;

	// these cant be in the ecs because they need an order of execution
	// this is where a jobs system comes in I think, I guess you make a strand
	// and at each level it can be multithreaded?
	// I guess each loop through the entities actually could queue work in a thread pool
	// and then exeacute all 100000 work items

	void on(event_Shutdown& e)
	{
		modules.destroy();
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
	WindowConfig windowConfig = {
		"Winter Framework Testbed", 1280, 720
	};

	archetype plugins = make_archetype(
	{
		make_component<Window>(),
		make_component<PhysicsWorld>()
	});

	app.modules = entities()
		.create(plugins)
		.set<Window>(windowConfig, &events());

	events().handle<event_Shutdown>(&app);
	events().handle<event_MouseMove>(&app);

	PhysicsWorld& world = app.modules.get<PhysicsWorld>();

	CreateTexturedBox(world);

	app.systems.push_back(new PhysicsInterpolationSystem());
	app.systems.push_back(new ForceTwoardwsMouseSystem());

	Time::SetFixedTime(1.0f);

	// setup imgui window

	SDL_Window* window = app.modules.get<Window>().m_window;
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);

	unsigned ff;
	glCreateBuffers(1, &ff);

	std::cout << "[INFO] OpenGL renderer: "
			<< glGetString(GL_RENDERER)
			<< std::endl;

}

float totalTime = 0.f;

bool loop()
{


	Time::UpdateTime();

	auto [window, world] = entities().query<Window, PhysicsWorld>().first();
 
	// main loop

	totalTime += Time::DeltaTime();

	if (totalTime > Time::FixedTime())
	{
		totalTime = 0;

		for (System* system : app.systems)
		{
			system->FixedUpdate();
		}

		// This needs to be under here for the last transform loop
		// to interpolate correctly

		world.Step(Time::FixedTime());
	}

	for (System* system : app.systems)
	{
		system->Update();
	}

	window.BeginRender();

	for (auto [transform, sprite] : entities().query<Transform2D, Texture>())
	{
		window.DrawSprite(transform, &sprite);
	}

	window.EndRender();
	window.PumpEvents();

	return app.modules.is_alive();
}