#include "../Entry.h"
#include "../ext/RenderingSDL.h"

#include "../Entity.h"

#include "box2d/box2d.h"

struct Transform
{
	float x, y, r;
};

struct Renderable
{
	b2AABB m_aabb;
	Sprite m_sprite;
};

// tie these together...

entity app;
float mouseX, mouseY;
struct
{
	void on(event_Shutdown& e)
	{
		app.destroy();
	}

	void on(event_MouseMove& e)
	{
		mouseX = e.x;
		mouseY = e.y;
	}
} EventCallbacks;

void setup()
{
	Window::InitRendering();

	WindowConfig windowConfig = {
		"Winter Framework Testbed", 1280, 720
	};

	archetype plugins = make_archetype(
	{
		make_component<Window>()
	});

	app = entities()
		.create(plugins)
		.set<Window>(windowConfig, &events());

	events().handle<event_Shutdown>(&EventCallbacks);
	events().handle<event_MouseMove>(&EventCallbacks);

	entities().create<Sprite>()
			  .set<Sprite>("C:/Users/Iain/Pictures/boss1 copy123.bmp");
}

bool loop()
{
	Window& window = app.get<Window>();

	window.BeginRender();

	SpriteTransform2D t;
	t.x = mouseX;
	t.y = mouseY;
	t.r += .001f;

	for (auto [sprite] : entities().query<Sprite>())
	{
		window.DrawSprite(t, sprite);
	}

	window.EndRender();

	window.PollEvents();

	return app.is_alive();
}