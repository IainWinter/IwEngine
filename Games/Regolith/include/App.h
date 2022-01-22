#pragma once 

// for event recording
#include "Events.h"
//#include "iw/util/reflection/serialization/Serializer.h"
//#include "iw/reflected/reflected.h"

#include "iw/engine/EntryPoint.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/physics/Dynamics/ImpulseSolver.h"
#include "iw/physics/Dynamics/SmoothPositionSolver.h"
#include "iw/math/noise.h"

#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Systems/PhysicsSystem.h"
#include "iw/engine/Systems/EntityCleanupSystem.h"

#include "Layers/Game_Layer.h"
#include "Layers/Game_UI_Layer.h"
#include "Layers/Menu_Title_Layer.h"
#include "Layers/Menu_Highscores_Layer.h"
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
	iw::FontMap* m_fonts;

	Menu_Title_Layer* m_menus;
	Game_Layer*       m_game;
	Game_UI_Layer*    m_gameUI;

	StateName m_state;

	void ChangeState(StateName state)
	{
		m_state = state;
		Bus->push<StateChange_Event>(state);
	}

public:
	App()
		: iw::Application ()
		, m_fonts  (nullptr)
		, m_menus  (nullptr)
		, m_game   (nullptr)
		, m_gameUI (nullptr)
		, m_state  (StateName::IN_MENU)
	{}

	void ChangeToPauseMenu()
	{
		m_menus->BackButtonFunc = [this]() { ChangeBackToGame(); };
		m_menus->BackButtonTarget = MenuTarget::GAME;
		m_menus->SetViewSettings();
		PopLayer(m_game->sand);
		PopLayer(m_game->sand_ui_laserCharge);
		PopLayer(m_game);
		//PopLayer(m_gameUI);
		Input->SetContext("menu");
		Physics->Paused = true;
		ChangeState(StateName::IN_GAME_THEN_MENU);
	}

	void ChangeBackToGame()
	{
		m_menus->BackButtonFunc = {};
		m_menus->BackButtonTarget = MenuTarget::DEFAULT;
		m_menus->SetViewGame(); // some random place with stars
		PushLayer(m_game->sand);
		PushLayer(m_game->sand_ui_laserCharge);
		PushLayer(m_game);
		//PushLayer(m_gameUI);
		Input->SetContext("game");
		Physics->Paused = false;
		ChangeState(StateName::IN_GAME);
	}

	int Initialize(iw::InitOptions& options) override;
};
