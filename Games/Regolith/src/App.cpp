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

void App::SetState(
	GameState* state)
{
	if (m_stateStack.size() > 0)
	{
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

App::App() : iw::Application()
{
	m_currentState = nullptr;
	m_gamePlay = nullptr;
	m_gamePause = nullptr;
	m_gamePost = nullptr;

	PushLayer<StaticLayer>();
	PushLayer<UI_Render_Layer>();

	m_gamePause = new GameState("Pause menu", GAME_PAUSE_STATE);
	m_gamePause->Layers.push_back(new Menu_Pause_Layer());
	m_gamePause->OnChange = [&]()
	{
		Physics->Paused = true;
		//Window()->SetCursor(true);
		Input->SetContext("Menu");
	};

	Console->QueueCommand("game-start");
}

int App::Initialize(
	iw::InitOptions& options)
{
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

	iw::ref<iw::Context> context_menu = Input->CreateContext("Menu");
	context_menu->MapButton(iw::LMOUSE, "+execute");
	context_menu->MapButton(iw::ESCAPE, "escape");

	iw::ref<iw::Device> mouse        = Input->CreateDevice<iw::Mouse>();
	//iw::ref<iw::Device> keyboard     = Input->CreateDevice<iw::Keyboard>();
	//iw::ref<iw::Device> mouse_raw    = Input->CreateDevice<iw::RawMouse>();
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
				case GAME_PAUSE_STATE:
					PopState();
					break;
				default:
					PushState(m_gamePause);
					break;
			}
		}

		else
		if (command.Verb == "game-over")
		{
			Bus->push<StateChange_Event>(GAME_OVER_STATE);

			if (m_gamePlay)
			{
				m_gamePlay->Layers.push_back(PushLayer<Menu_Fadeout_Layer>(4.f));
			}

			float time = iw::TotalTime();
			Task->coroutine([&, time]()
			{
				bool done = iw::TotalTime() - time > 0;

				if (done)
				{
					Game_Layer* game = GetLayer<Game_Layer>("Game");
					ScoreSystem* score_s = nullptr;
					if (game) score_s = game->GetSystem<ScoreSystem>("Score");

					int finalScore = iw::randi(10000);
					if (score_s) finalScore = score_s->Score;

					Menu_PostGame_Layer* menu = new Menu_PostGame_Layer(finalScore);

					m_gamePost = new GameState("Post game menu");
					m_gamePost->Layers.push_back(menu);
					m_gamePost->OnChange = [&]()
					{
						Physics->Paused = true;
						//Window()->SetCursor(true);
						Input->SetContext("Menu");
					};

					if (m_gamePlay)
					{
						DestroyState(m_gamePlay);
					}

					SetState(m_gamePost);


				}

				return done;
			});
		}

		else
		if (command.Verb == "game-start")
		{
			if (m_gamePlay)
			{
				LOG_WARNING << "Game already started";
				return true;
			}

			iw::SandLayer* sand          = new iw::SandLayer(2, 1, 800, 800, 4, 4, false);
			iw::SandLayer* sand_ui_laser = new iw::SandLayer(1, 1, 40, 40);
			sand         ->m_updateDelay = 1 / 144.f;
			sand_ui_laser->m_updateDelay = 1 / 60.f;

			Game_Layer*    game    = new Game_Layer   (sand, sand_ui_laser);
			Game_UI_Layer* game_ui = new Game_UI_Layer(sand, sand_ui_laser);

			m_gamePlay = new GameState("Game play", GAME_RESUME_STATE);
			m_gamePlay->Layers.push_back(sand);
			m_gamePlay->Layers.push_back(sand_ui_laser);
			m_gamePlay->Layers.push_back(game);
			m_gamePlay->Layers.push_back(game_ui);

			m_gamePlay->OnChange = [&]()
			{
				Physics->Paused = false;
				//Window()->SetCursor(false);
				Input->SetContext("Game");
			};

			if (m_gamePost)
			{
				DestroyState(m_gamePost);
			}

			SetState(m_gamePlay);
			Bus->push<StateChange_Event>(GAME_START_STATE);
		}

		else
		if (command.Verb == "quit")
		{
			Stop();
		}

		return false;
	});

	return Application::Initialize(options);
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	options.WindowOptions = iw::WindowOptions {
		900,
		900,
		true,
		iw::DisplayState::NORMAL
	};

	return new App();
}
