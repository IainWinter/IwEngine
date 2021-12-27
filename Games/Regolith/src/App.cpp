#include "App.h"

#include <combaseapi.h>

// State ideas

// There are some different states that the progra cna be in, idealy these would only be differnt layers being in the app at a time



// So the game running state is, the two sand layers and the game layer
// There is a menu upgrade state which is only the post game menus layer
// A pause state, the game state in the background (paused), and the pause menu layer
// A main menu state, menu layer

// So list of Layers

// Sand Layer
// Game Layer
// Pause Menu Layer
// Main Menu Layer
// Post game Menu Layer

// If ui layer is differnt thatl work i think bc it holds every thing drawn by the game

void App::ApplyState(GameState* state)
{
	LOG_INFO << "Set game state " << state->Name;

	if (m_currentState)
	for (iw::Layer* layer : m_currentState->Layers)
	{
		PopLayer(layer);
	}

	for (iw::Layer* layer : state->Layers)
	{
		PushLayer(layer);
	}

	if (state->OnChange) 
	{
		state->OnChange();
	}

	Bus->push<StateChange_Event>(state->State);
	m_currentState = state;
}

void App::DestroyState(
	GameState*& state)
{
	std::set<iw::Layer*> layers;

	if (state)
	{
		for (iw::Layer* layer : state->Layers)
		{
			layers.insert(layer);
		}

		if (state == m_currentState)
		{
			m_currentState = nullptr;
		}
		
		delete state;
		state = nullptr;
	}

	for (iw::Layer* layer : layers)
	{
		DestroyLayer(layer);
	}
}

void App::SetState(
	GameState* state)
{
	while (m_stateStack.size() > 0)
	{
		DestroyState(m_stateStack.top());
		m_stateStack.pop();
	}

	PushState(state);
}

void App::PushState(
	GameState* state)
{
	ApplyState(state);
	m_stateStack.push(state);
}

void App::PopState() 
{
	if (m_stateStack.size() > 0)
	{
		m_stateStack.pop();
		if (m_stateStack.size() > 0)
		{
			ApplyState(m_stateStack.top());
		}
	}
}

GameState* App::MakeState_Title()
{
	GameState* state = new GameState("Title Menu", StateName::NONE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = { 
		//new Menu_Title_Layer()
	};

	return state;
}

GameState* App::MakeState_Main()
{
	GameState* state = new GameState("Main Menu", StateName::NONE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = {
		//new Menu_Main_Layer()
	};

	return state;
}

GameState* App::MakeState_Play()
{
	GameState* state = new GameState("Game Play", StateName::GAME_RESUME_STATE);
	state->OnChange = [&]()
	{
		Physics->Paused = false;
		Input->SetContext("Game");
	};

	iw::SandLayer* sand          = new iw::SandLayer(2, 1, 800, 800, 4, 4, false);
	iw::SandLayer* sand_ui_laser = new iw::SandLayer(1, 1, 40,  40);
	sand         ->m_updateDelay = 1 / 144.f;
	sand_ui_laser->m_updateDelay = 1 / 60.f;
	
	Game_Layer*    game    = new Game_Layer   (sand, sand_ui_laser);
	Game_UI_Layer* game_ui = new Game_UI_Layer(sand, sand_ui_laser);
	
	state->Layers = {
		sand, 
		sand_ui_laser, 
		game, 
		game_ui
	};
	
	return state;
}

GameState* App::MakeState_Highscores()
{
	GameState* state = new GameState("Menu Highscores", StateName::NONE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = {
		new Menu_Highscores_Layer(m_finalScore)
	};

	return state;
}

GameState* App::MakeState_Settings()
{
	GameState* state = new GameState("Menu Settings", StateName::GAME_PAUSE_STATE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = {
		new Menu_Pause_Layer()
	};

	return state;
}

GameState* App::MakeState_Credits()
{
	GameState* state = new GameState("Menu Credits", StateName::NONE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = {
		//new Menu_Credits_Layer()
	};

	return state;
}

GameState* App::MakeState_Upgrade()
{
	GameState* state = new GameState("Menu Upgrade", StateName::NONE);
	state->OnChange = [&]()
	{
		Physics->Paused = true;
		Input->SetContext("Menu");
	};
	state->Layers = {
		new Menu_Upgrade_Layer(m_finalScore)
	};

	return state;
}

int App::Initialize(
	iw::InitOptions& options)
{
	PushLayer<StaticLayer>();
	PushLayer<Audio_Layer>();
	PushLayer<UI_Render_Layer>();

	int err = Application::Initialize(options);
	if (err) return err;

	m_fonts = new iw::FontMap();
	m_fonts->Load("verdana", 18, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana", 36, "fonts/ttf/verdana.ttf");
	m_fonts->Load("verdana", 92, "fonts/ttf/verdana.ttf");

	PushLayer<iw::ImGuiLayer>(Window, m_fonts)->BindContext();

	PushLayer<iw::DebugLayer>();

	// add default cells

	iw::Cell c;
	c.Type        = iw::CellType::LIGHTNING;
	c.Props       = iw::CellProp::NONE;
	c.Style       = iw::CellStyle::SHIMMER;
	c.StyleColor  = iw::Color(.1, .1, .1, 0);
	c.Color       = iw::Color::From255(212, 194, 252);
	c.life        = .1;
	
	iw::Cell::SetDefault(iw::CellType::LIGHTNING, c);

	//ImGui::SetCurrentContext((ImGuiContext*)options.ImGuiContext);

	m_gamePause = new GameState("Pause menu", StateName::GAME_PAUSE_STATE);
	m_gamePause->Layers.push_back(new Menu_Pause_Layer());
	m_gamePause->OnChange = [&]()
	{
		Physics->Paused = true;
		//Window()->SetCursor(true);
		//Renderer->Device->SetVSync();
		Input->SetContext("Menu");
	};

	Console->QueueCommand("set-state highscores");

	// create a guid for the installation of this app

	//GUID install_id;

	//if (iw::FileExists("install_id.txt"))
	//{
	//	std::string str = iw::ReadFile("install_id.txt");
	//	std::wstring s;
	//	s.assign(str.begin(), str.end());

	//	install_id = s;
	//}

	//else
	//{
	//	GUID& id = install_id;
	//	char szGuid[40] = { 0 };
	//	CoCreateGuid(&id);

	//	sprintf(szGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", 
	//		id.Data1,    id.Data2,    id.Data3, 
	//		id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
	//		id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);

	//	iw::WriteFile("install_id.txt", szGuid);
	//}



	iw::ref<iw::Context> context_game = Input->CreateContext("Game");
	context_game->MapButton(iw::D     , "+right");
	context_game->MapButton(iw::A     , "-right");
	context_game->MapButton(iw::W     , "+up");
	context_game->MapButton(iw::S     , "-up");
	context_game->MapButton(iw::LMOUSE, "+fire");
	context_game->MapButton(iw::RMOUSE, "+alt-fire");
	context_game->MapButton(iw::ESCAPE, "escape");
	context_game->MapButton(iw::X     , "toolbox");

	context_game->MapButton(iw::E , "+a"); // buttons for upgrades
	context_game->MapButton(iw::Q , "+b");
	context_game->MapButton(iw::F , "+x");
	context_game->MapButton(iw::C , "+y");

	iw::ref<iw::Context> context_menu = Input->CreateContext("Menu");
	context_menu->MapButton(iw::LMOUSE, "+execute");
	context_menu->MapButton(iw::ESCAPE, "escape");

	iw::ref<iw::Device> mouse        = Input->CreateDevice<iw::Mouse>();
	iw::ref<iw::Device> keyboard_raw = Input->CreateDevice<iw::RawKeyboard>();

	context_game->AddDevice(mouse);
	context_game->AddDevice(keyboard_raw);

	context_menu->AddDevice(mouse);
	context_menu->AddDevice(keyboard_raw);

	// menu states could be switched to through commands

	// game-play   -- starts game
	// game-end    -- ends game, brings you to post game menus
	// game-exit   -- exits game to main menu
	// game-exit d -- exits game to desktop
	// game-pause  -- opens escape menu

	Console->AddHandler([&](
		const iw::Command& command)
	{
		LOG_INFO << command.Original;

		if (command.Verb == "escape")
		{
			switch (m_currentState->State)
			{
				case StateName::GAME_PAUSE_STATE:
					PopState();
					break;
				default:
					PushState(m_gamePause);
					break;
			}
		}

		else
		if (command.Verb == "fade")
		{
			float fadeTime  = command.TokenCount > 0 ? command.Tokens[0].Float : 1.f;
			float fadeDelay = command.TokenCount > 1 ? command.Tokens[1].Float : 0.f;
			float fadeTotalTime = fadeTime + fadeDelay;

			iw::Layer* fadeLayer = PushLayer<Menu_Fadeout_Layer>(fadeTime, fadeDelay);

			float time = iw::RawTotalTime();
			Task->coroutine(
				[this, time, fadeTotalTime, fadeLayer]()
			{
				bool done = iw::RawTotalTime() - time > fadeTotalTime;
				if (done) PopLayer(fadeLayer);
				return done;
			});
		}

		else
		if (command.Verb == "set-state")
		{
			std::string stateName = command.Tokens[0].String;

			     if (stateName == "title")      SetState(MakeState_Title());
			else if (stateName == "main")       SetState(MakeState_Main());
			else if (stateName == "play")       SetState(MakeState_Play());
			else if (stateName == "highscores") SetState(MakeState_Highscores());
			else if (stateName == "settings")   SetState(MakeState_Settings());
			else if (stateName == "credits")    SetState(MakeState_Credits());
			else if (stateName == "upgrade")    SetState(MakeState_Upgrade());

			else
			{
				LOG_ERROR << "[set-state] invalid state";
			}
		}

		else
		if (command.Verb == "push-state")
		{
			std::string stateName = command.Tokens[0].String;

			if (stateName == "settings")   PushState(MakeState_Settings());

			else
			{
				LOG_ERROR << "[push-state] invalid state";
			}
		}

		else
		if (command.Verb == "push-state")
		{
			PopState();
		}

		else
		if (command.Verb == "final-score")
		{
			m_finalScore = command.Tokens[0].Int;
		}

		else
		if (command.Verb == "quit")
		{
			Stop();
		}

		return false;
	});

	return 0;
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	//options.AssetRootPath = "_assets/";

	//tions.FrameDelay = 0.05f;

	options.WindowOptions = iw::WindowOptions {
		816,
		1039,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}
