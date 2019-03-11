#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"

#include "iw/events/functional/eventbus.h"
#include "iw/engine/Events/Event.h"

using namespace iwevt::functional;

using Bus = eventbus<
	IwEngine::Event&,
	IwEngine::WindowResizedEvent&,
	IwEngine::MouseButtonEvent&,
	IwEngine::MouseMovedEvent&>;

class Test {
public:
	int t = 5;

	void BindEvents(Bus& bus) {
		bus.subscribe<IwEngine::WindowResizedEvent&>(this, &Test::OnWindowResized);
		bus.subscribe<IwEngine::MouseButtonEvent&>(this, &Test::OnMouseButton);
		bus.subscribe<IwEngine::MouseMovedEvent&>(this, &Test::OnMouseMoved);
	}

private:
	void OnWindowResized(
		IwEngine::WindowResizedEvent& e)
	{
		LOG_DEBUG << "Window resized " 
			<< e.Width << ", " << e.Height;
	}

	void OnMouseButton(
		IwEngine::MouseButtonEvent& e)
	{
		LOG_DEBUG << "Mouse button "
			<< e.Button << " " << e.State;
	}

	void OnMouseMoved(
		IwEngine::MouseMovedEvent& e) 
	{
		LOG_DEBUG << "Mouse moved " 
			<< e.X << ", " << e.Y;
	}
};

class Game : public IwEngine::Application {
public:
	Game() {
		PushOverlay(new IwEngine::ImGuiLayer());
	}

	void Run() override {		
		Bus b;

		Test* t = new Test();
		t->BindEvents(b);

		IwEngine::WindowResizedEvent e(100, 200);
		b.emit<IwEngine::WindowResizedEvent&>(e);

		Application::Run();
	}
};

IwEngine::Application* CreateApplication() {
	return new Game();
}

//Update loop

//Application update
//-> Update layers
//-> Update window
//   -> Get raw event 
//   -> Translate to engine event 
//   -> Pass to application
//-> Pass to Layer bus
//   -> Dispatch to correct layers in correct order
//-> Render window
