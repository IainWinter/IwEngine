#pragma once 

// for event recording
#include "Events.h"
//#include "iw/util/reflection/serialization/Serializer.h"
//#include "iw/reflected/reflected.h"

#include "iw/engine/EntryPoint.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "Layers/Game_Layer.h"
#include "Layers/Game_UI_Layer.h"
#include "Layers/Menu_Pause_Layer.h"
#include "Layers/Menu_PostGame_Layer.h"

#include "State.h"
#include <stack>

struct StaticLayer : iw::Layer
{
	StaticLayer()
		: iw::Layer("Static")
	{}

	int Initialize() override
	{
		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		// this shouldnt really be here
		// load all assets into global variables
		if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
		{
			return e;
		}

		return Layer::Initialize();
	}
};

struct PostGameLayer : iw::Layer
{
	PostGameLayer() : iw::Layer("Post game") {}

	void PostUpdate();
};

struct UI_Layer : iw::Layer
{
	UI_Layer() : iw::Layer("UI") {}

	void PostUpdate()
	{
		Space->Query<UIScreen>().Each([&](iw::EntityHandle handle, UIScreen* ui)
		{
			ui->Draw(Renderer);
		});
	}
};

class App : public iw::Application {
private:
	GameState* game_play;
	GameState* game_pause;
	GameState* game_post;

	std::stack<GameState*> StateStack;
	GameState* CurrentState;

	void ApplyState(GameState* state);
	void SetState  (GameState* state);
	void PushState (GameState* state);
	void PopState  ();
	void DestroyStates(std::vector<GameState*> states);
public:
	App();
	int Initialize(iw::InitOptions& options) override;
};
