#include "iw/engine/EntryPoint.h"
#include "Layers/GameLayer.h"

#include "imgui/imgui.h"

#include "iw/entity2/Space.h"
#include "iw/util/memory/pool_allocator.h"

class Game
	: public IwEngine::Application
{
public:
	Game() {
		InputManager.CreateDevice<IwInput::Mouse>();
		//InputManager.CreateDevice<IwInput::RawKeyboard>();

		PushLayer<GameLayer>();
	}

	int Initialize(
		IwEngine::InitOptions& options) override
	{
		Application::Initialize(options);

		ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

		return 0;
	}
};

struct int3 {
	int x, y, z;
};

struct float3 {
	float x, y, z;
};

struct long3 {
	long x, y, z;
};

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options)
{
	IwEntity2::Space space;
	IwEntity2::Entity e1 = space.CreateEntity();
	IwEntity2::Entity e2 = space.CreateEntity();
	space.CreateComponent<int3>(e1);
	float3* f = space.CreateComponent<float3>(e2);
	f->x = 0.0f;
	f->y = 1.0f;
	f->z = 2.0f;
	long3* l = space.CreateComponent<long3> (e2);
	l->x = 100234234;

	space.DestroyComponent<float3>(e2);

	options.WindowOptions = IwEngine::WindowOptions {
		1280,
		720,
		true,
		IwEngine::NORMAL,
	};

	return new Game();
}
