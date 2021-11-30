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
#include "Layers/Menu_Fadeout_Layer.h"
#include "Layers/Menu_Upgrade_Layer.h"
#include "Layers/Audio_Layer.h"

#include "State.h"
#include <stack>

#include "iw/util/io/File.h"

struct StaticLayer : iw::Layer
{
	StaticLayer()
		: iw::Layer("Static")
	{}

	int Initialize() override
	{
		PushSystem<iw::PhysicsSystem>();
		PushSystem<iw::EntityCleanupSystem>();

		if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
		{
			return e;
		}

		Physics->AddSolver(new iw::SmoothPositionSolver());
		Physics->AddSolver(new iw::ImpulseSolver());

		return Layer::Initialize();
	}
};

struct UI_Render_Layer : iw::Layer
{
	UI_Render_Layer() : iw::Layer("UI render") {}

	void PostUpdate() override
	{
		int i = 0;
		Space->Query<UI_Screen>().Each([&](iw::EntityHandle handle, UI_Screen* ui)
		{
			Renderer->SetLayer(i++);
			ui->Draw(Renderer);
		});
	}
};

class App
	: public iw::Application
{
private:
	GameState* m_gamePlay;
	GameState* m_gamePause;
	GameState* m_gameHighscore;
	GameState* m_gameUpgrade;
	GameState* m_fadeout;

	std::stack<GameState*> m_stateStack;
	GameState* m_currentState;

	int m_finalScore; // this is needed to pass to the game-over/game-upgrade layers
				   // little messy to have here...

	void ApplyState  (GameState* state);
	void SetState    (GameState* state);
	void PushState   (GameState* state);
	void PopState    ();
	void DestroyState(GameState*& states);

public:
	App()
		: iw::Application ()
		, m_currentState  (nullptr)
		, m_gamePlay      (nullptr)
		, m_gamePause     (nullptr)
		, m_gameHighscore (nullptr)
		, m_gameUpgrade   (nullptr)
		, m_fadeout       (nullptr)
	{
		// for debug, if game starts on game-over
		m_finalScore = iw::randi(10000);
	}

	int Initialize(iw::InitOptions& options) override;
};
