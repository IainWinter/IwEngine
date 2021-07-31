#include "iw/engine/EntryPoint.h"
#include "Sand.h"

using namespace iw;

struct App : Application 
{
	App() {
		//Audio = REF<AudioSpaceStudio>("assets/sounds/");

		iw::ref<Context> context = Input->CreateContext("Editor");
		iw::ref<Device> m = Input->CreateDevice<Mouse>();
		iw::ref<Device> k = Input->CreateDevice<RawKeyboard>();
		
		context->AddDevice(m);
		context->AddDevice(k);

		PushLayer<SandLayer>();
	}
};

Application* CreateApplication(
	InitOptions& options)
{
	options.WindowOptions = WindowOptions {
		1920,
		1080,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}
