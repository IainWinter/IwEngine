#include "App.h"

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

	if (CurrentState)
	for (iw::Layer* layer : CurrentState->Layers)
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
	CurrentState = state;
}

void App::SetState(
	GameState* state)
{
	if (StateStack.size() > 0)
	{
		StateStack.pop();
	}

	PushState(state);
}

void App::PushState(
	GameState* state)
{
	ApplyState(state);
	StateStack.push(state);
}

void App::PopState() 
{
	if (StateStack.size() > 0)
	{
		StateStack.pop();
		if (StateStack.size() > 0)
		{
			ApplyState(StateStack.top());
		}
	}
}

void App::DestroyStates(
	std::vector<GameState*> states)
{
	std::set<iw::Layer*> layers;

	for (GameState* state : states) 
	{
		for (iw::Layer* layer : state->Layers)
		{
			layers.insert(layer);
		}

		if (state == CurrentState)
		{
			CurrentState = nullptr;
		}
		
		delete state;
	}

	for (iw::Layer* layer : layers)
	{
		DestroyLayer(layer);
	}
}

App::App() : iw::Application()
{
	CurrentState = nullptr;
	game_play = nullptr;
	game_pause = nullptr;
	game_post = nullptr;

	PushLayer<StaticLayer>();
	PushLayer<UI_Layer>();

	Menu_PostGame_Layer* menu_postGame = new Menu_PostGame_Layer();

	game_post = new GameState("Post game menu");
	game_post->Layers.push_back(menu_postGame);

	Console->QueueCommand("game-start");
}

int App::Initialize(
	iw::InitOptions& options)
{
	iw::ref<iw::Context> context = Input->CreateContext("Game");
		
	context->MapButton(iw::D     , "+right");
	context->MapButton(iw::A     , "-right");
	context->MapButton(iw::W     , "+up");
	context->MapButton(iw::S     , "-up");
	context->MapButton(iw::LMOUSE, "+fire");
	context->MapButton(iw::RMOUSE, "+alt-fire");
	context->MapButton(iw::SPACE , "action");
	context->MapButton(iw::ESCAPE, "escape");

	// menu states could be switched to through commands

	// game-play   -- starts game
	// game-end    -- ends game, brings you to post game menus
	// game-exit   -- exits game to main menu
	// game-exit d -- exits game to desktop
	// game-pause  -- opens escape menu

	context->AddDevice(Input->CreateDevice<iw::Mouse>());
	context->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
	context->AddDevice(Input->CreateDevice<iw::RawMouse>());

	Console->AddHandler([&](
		const iw::Command& command)
	{
		LOG_INFO << command.Original;

		if (command.Verb == "escape")
		{
			switch (CurrentState->State)
			{
				case GAME_PAUSE_STATE:
					PopState();
					break;
				default:
					PushState(game_pause);
					break;
			}
		}

		else
		if (command.Verb == "game-over")
		{
			DestroyStates({ game_play });
			game_play = nullptr;

			SetState(game_post);
		}

		else
		if (command.Verb == "game-start")
		{
			if (game_play || game_pause)
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
	
			Menu_Pause_Layer* menu_pause = new Menu_Pause_Layer();

			game_play = new GameState("Game play", GAME_RESUME_STATE);
			game_play->Layers.push_back(sand);
			game_play->Layers.push_back(sand_ui_laser);
			game_play->Layers.push_back(game);
			game_play->Layers.push_back(game_ui);

			game_pause = new GameState("Pause menu", GAME_PAUSE_STATE);
			game_pause->Layers.push_back(menu_pause);
			
			game_play->OnChange = [&]() {
				Physics->Paused = false;
			};

			game_pause->OnChange = [&]() {
				Physics->Paused = true;
			};

			SetState(game_play);
			Bus->push<StateChange_Event>(GAME_START_STATE);
		}

		return false;
	});

	return Application::Initialize(options);
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	//options.AssetRootPath = "C:/dev/wEngine/_assets/";
	//options.AssetRootPath = "assets/";

	options.WindowOptions = iw::WindowOptions {
		800 + 38/2,
		1000 + 38,
		true,/*false,*/
		iw::DisplayState::NORMAL
	};

	return new App();
}

iw::Application* GetApplicationForEditor() {
	return new App();
}
