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
#include "Layers/Menu_Fadeout_Layer.h"
#include "Layers/Menu_Upgrade_Layer.h"
#include "Layers/Audio_Layer.h"
#include "Layers/Menu_Bg_Render_Layer.h"

#include "State.h"
#include "MenuState.h"
#include <stack>

#include "iw/util/io/File.h"

struct temp_PhysicsSystem
	: iw::SystemBase
{
	float accumulator;
	
	temp_PhysicsSystem()
		: iw::SystemBase("temp Physics")
		, accumulator(0.0f)
	{}

	void Update()
	{
		accumulator += iw::DeltaTime();

		float a = glm::clamp(accumulator / iw::RawFixedTime(), 0.f, 1.f);

		for (auto [transform, rigidbody] : entities().query<iw::Transform, iw::Rigidbody>())
		{
			if (rigidbody.IsKinematic)
			{
				transform.Position = iw :: lerp(rigidbody.LastTrans().Position, rigidbody.Transform.Position, a);
				transform.Scale    = iw :: lerp(rigidbody.LastTrans().Scale,    rigidbody.Transform.Scale,    a);
				transform.Rotation = glm::slerp(rigidbody.LastTrans().Rotation, rigidbody.Transform.Rotation, a);
			}
		}

		for (auto [transform, object] : entities().query<iw::Transform, iw::CollisionObject>())
		{
			transform.Position = iw :: lerp(transform.Position, object.Transform.Position, a);
			transform.Scale    = iw :: lerp(transform.Scale,    object.Transform.Scale,    a);
			transform.Rotation = glm::slerp(transform.Rotation, object.Transform.Rotation, a);
		}
	}

	void FixedUpdate()
	{
		accumulator = 0;
	}
};

struct StaticLayer : iw::Layer
{
	StaticLayer()
		: iw::Layer("Static")
	{}

	int Initialize() override
	{
		//PushSystem<iw::PhysicsSystem>();
		PushSystem<temp_PhysicsSystem>();
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

struct TileRender_Layer : iw::Layer
{
	iw::SandLayer* m_sand;

	TileRender_Layer(
		iw::SandLayer* sand
	)
		: iw::Layer ("Tile Render")
		, m_sand    (sand)
	{}

	void PreUpdate() override
	{
		std::vector<std::tuple<iw::Transform*, iw::Tile*, iw::MeshCollider2*>> tiles;

		for (auto [entity, transform, tile] : entities().query<iw::Transform, iw::Tile>().with_entity())
		{
			iw::MeshCollider2* collider = entity.has<iw::MeshCollider2>()
				? &entity.get<iw::MeshCollider2>()
				: nullptr;

			tiles.emplace_back(&transform, &tile, collider);
		}

		m_sand->PasteTiles(tiles);
	}

	void PostUpdate() override
	{
		m_sand->RemoveTiles();
	}
};

class App
	: public iw::Application
{
private:
	iw::FontMap* m_fonts;

	Menu_Title_Layer*  m_menus;
	Game_Layer*        m_game;
	Menu_GameUI_Layer* m_gameUI;
	TileRender_Layer*  m_tileRender;

	StateName m_state;

	void ChangeState(StateName state)
	{
		m_state = state;
		Bus->push<StateChange_Event>(state);
	}

public:
	App()
		: iw::Application ()
		, m_fonts         (nullptr)
		, m_menus         (nullptr)
		, m_game          (nullptr)
		, m_gameUI        (nullptr)
		, m_tileRender    (nullptr)
		, m_state         (StateName::IN_MENU)
	{}

	void ChangeToPauseMenu()
	{
		m_menus->PushBackState(MenuTarget::GAME, [this]() { ChangeBackToGame(); });
		m_menus->SetViewPause();
		PopLayer(m_game->sand);
		PopLayer(m_game->sand_ui_laserCharge);
		PopLayer(m_game);
		//PopLayer(m_gameUI);
		Physics->Paused = true;

		Input->SetContext("menu");
		ChangeState(StateName::IN_GAME_THEN_MENU);
	}

	void ChangeBackToGame()
	{
		m_menus->PushBackState(MenuTarget::DEFAULT);
		m_menus->SetViewGame(); // some random place with stars
		PushLayerFront(m_game->sand);
		PushLayerFront(m_game->sand_ui_laserCharge);
		PushLayerFront(m_game);
		Physics->Paused = false;

		Input->SetContext("game");
		ChangeState(StateName::IN_GAME);
	}

	void ChangeToTitleScreen() // never from game
	{
		m_menus = PushLayer<Menu_Title_Layer>();
		PushLayer<Menu_Bg_Render_Layer>();

		Input->SetContext("menu");
		m_state = StateName::IN_MENU;
	}

	void ChangeToGame()
	{
		iw::SandLayer* sand          = new iw::SandLayer(2, 1, 800, 800, 4, 4, false);
		iw::SandLayer* sand_ui_laser = new iw::SandLayer(1, 1, 40,  40);
		sand         ->m_updateDelay = 1 / 144.f;
		sand_ui_laser->m_updateDelay = 1 / 60.f;
	
		m_game   = new Game_Layer       (sand, sand_ui_laser);
		m_gameUI = new Menu_GameUI_Layer(sand, sand_ui_laser, m_menus->bg);

		m_tileRender = new TileRender_Layer(sand);

		PushLayer(sand);
		PushLayer(sand_ui_laser);
		PushLayer(m_game);
		PushLayer(m_gameUI);
		PushLayer(m_tileRender);

		Input->SetContext("game");
		m_state = StateName::IN_GAME;
	}

	void ChangeToPost()
	{
		iw::TextureAtlas movie = m_game->GetDeathMovie();
		m_menus->SetViewPost(movie/* here should be a flag for if it's post game or from main menu */);

		DestroyLayer(m_gameUI);
		DestroyLayer(m_game);
		DestroyLayer(m_game->sand);
		DestroyLayer(m_game->sand_ui_laserCharge);
		DestroyLayer(m_tileRender);

		Input->SetContext("menu");
		m_state = StateName::IN_MENU;
	}

	void SetState(
		const std::string& stateName)
	{
		     if (stateName == "menus") ChangeToTitleScreen();
		else if (stateName == "game")  ChangeToGame();
		else if (stateName == "post")  ChangeToPost();
		else
		{
			LOG_ERROR << "[set-state] invalid state";
		}
	}

	int Initialize(iw::InitOptions& options) override;
};
