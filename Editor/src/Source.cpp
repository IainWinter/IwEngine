#include "iw/engine/Application.h"
#include "iw/engine/Events/Events.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/engine/Layers/ToolLayer.h"
#include "iw/engine/Systems/SpaceInspectorSystem.h"
#include <string>

//#include "iw/math/matrix.h"

using GETAPP_FUNC = iw::Application*(*)(iw::InitOptions&);

HINSTANCE LoadDll(
	std::wstring dll) 
{
	HINSTANCE inst = nullptr;

	inst = LoadLibrary(dll.c_str());

	if (!inst) {
		LOG_ERROR << "Failed to load library " << dll.c_str();
	}

	return inst;
}

template<
	typename _t>
_t LoadFunction(
	HINSTANCE hInst, 
	std::string function) 
{
	_t funcPtr = (_t)GetProcAddress(hInst, function.c_str());

	if (!funcPtr) {
		LOG_ERROR << "Failed to load function " << function;
	}

	return funcPtr;
}

struct InspectorLayer : iw::Layer
{
	InspectorLayer() : iw::Layer("Inspector")
	{}

	int Initialize() override
	{
		PushSystem<iw::SpaceInspectorSystem>();

		return Layer::Initialize();
	}
};

int main()
{
	std::wstring projectName = L"PhysicsProject";

	std::wstringstream buf;
	
	#ifdef IW_DEBUG
			buf << L"C:/dev/IwEngine/Games/";
			buf << projectName;
			buf << L"/bin/Debug.windows.x86_64/";
			buf << projectName;
			buf << L".dll";
	#else
			buf << L"C:/dev/IwEngine/Games/";
			buf << projectName;
			buf << "/bin/Release.windows.x86_64/";
			buf << projectName;
			buf << ".dll";
	#endif
	
	HINSTANCE   gameInst  = LoadDll(buf.str().c_str());
	GETAPP_FUNC createApp = LoadFunction<GETAPP_FUNC>(gameInst, "CreateApplication");

	iw::InitOptions options;
	iw::Application* app = createApp(options);

	app->PushLayerFront<InspectorLayer>();
	app->PushLayerFront<iw::ImGuiLayer>(app->Window);

	app->Initialize(options);
	app->Run();

	return 0;
}
