#include "iw/engine/EntryPoint.h"
#include "iw/engine/ImGui/ImGuiLayer.h"

class Game : public IwEngine::Application {
public:
	Game() {
		PushOverlay(new IwEngine::ImGuiLayer());
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
