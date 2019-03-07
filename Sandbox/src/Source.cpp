#include "EntryPoint.h"

#include <gl/glew.h>
#include <gl/wglew.h>

#include "iw/signal.h"
#include "iw/functional/callback.h"
#include "iw/functional/signal.h"

class TestLayer
	: public IwEngine::Layer
{
private:
	float r, g, b;
public:
	TestLayer()
		: IwEngine::Layer("Test") 
	{
		r = g = b = 0;
	}

	int Initilize() override {
		glClearColor(0.5f, 0.3f, 0.87f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		return 0;
	}

	void Update() override {
		r += .001f;
		g += .003f;
		b += .002f;

		if (r > 1.0f) r = 0;
		if (g > 1.0f) g = 0;
		if (b > 1.0f) b = 0;
	}

	void OnEvent(
		IwEngine::Event& e) override
	{
		if (e.Type == IwEngine::MouseMoved) {
			glClearColor(r, g, b, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
		}
	}
};

class Game : public IwEngine::Application {
public:
	Game() {
		PushLayer(new TestLayer());
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}
