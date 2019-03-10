#include "iw/engine/EntryPoint.h"
#include "gl/glew.h"

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
		return 0;
	}

	void Update() override {
		glClearColor(r, g, b, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		r += .0001f;
		g += .0002f;
		b += .0003f;

		if (r > 1.0f) r = 0;
		if (g > 1.0f) g = 0;
		if (b > 1.0f) b = 0;
	}

	void OnEvent(
		IwEngine::Event& e) override
	{
		if (e.Type == IwEngine::MouseMoved) {
			r = 0;
			g = 0;
			b = 0;
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
