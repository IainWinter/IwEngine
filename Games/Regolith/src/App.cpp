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

App::App() : iw::Application() 
{
	state = RUN_STATE;
	menu = nullptr;

	int cellSize  = 2;
	int cellMeter = 1;
	bool drawWithMouse = false;
	
	sand = new iw::SandLayer(cellSize, cellMeter, 800, 800, 4, 4, drawWithMouse);
	sand->m_updateDelay = 1/144.f;

	sand_ui_laser = new iw::SandLayer(1, 1, 40, 40);
	sand_ui_laser->m_updateDelay = .016f;

	game = new Game_Layer(sand, sand_ui_laser);

	PushLayer(sand);
	PushLayer(sand_ui_laser);
	PushLayer(game);
}

int App::Initialize(
	iw::InitOptions& options)
{
	// load all assets into global variables
	if (int e = LoadAssets(Asset.get(), Renderer->Now.get()))
	{
		return e;
	}

	iw::ref<iw::Context> context = Input->CreateContext("Game");
		
	context->MapButton(iw::SPACE , "action");
	context->MapButton(iw::D     , "+right");
	context->MapButton(iw::A     , "-right");
	context->MapButton(iw::W     , "+forward");
	context->MapButton(iw::S     , "-forward");
	context->MapButton(iw::ESCAPE, "esc-menu");

	context->AddDevice(Input->CreateDevice<iw::Mouse>());
	context->AddDevice(Input->CreateDevice<iw::RawKeyboard>());
	context->AddDevice(Input->CreateDevice<iw::RawMouse>());

	Console->AddHandler([&](
		const iw::Command& command)
	{
		if (command.Verb == "esc-menu" && !game->showGameOver)
		{
			switch (state)
			{
				case RUN_STATE:
				{
					PopLayer(sand);
					PopLayer(sand_ui_laser);

					if (!menu) menu = new MenuLayer();
					PushLayer(menu);

					state = PAUSE_STATE;
					Bus->push<StateChange_Event>(PAUSE_STATE);

					Physics->Paused = true;

					break;
				}
				case PAUSE_STATE:
				{
					PushLayer(sand);
					PushLayer(sand_ui_laser);

					if (menu) PopLayer(menu);

					state = RUN_STATE;
					Bus->push<StateChange_Event>(RESUME_STATE);

					Physics->Paused = false;

					break;
				}
			}
		}

		else if (command.Verb == "action" && game->showGameOver)
		{
			Bus->push<StateChange_Event>(RUN_STATE);
		}

		return false;
	});

	return Application::Initialize(options);
}

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	//options.AssetRootPath = "C:/dev/IwEngine/_assets/";
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

void MenuLayer::PostUpdate()
{
	UIScreen screen(Renderer->Width(), Renderer->Height(), 10);

	UI* background = screen.CreateElement(A_mesh_menu_background);
	background->width  = screen.width;
	background->height = screen.height;
	background->z = -5;

	UI* pause_menu = screen.CreateElement(A_mesh_menu_pause);
	pause_menu->height = 600;
	pause_menu->width  = 600;
	pause_menu->z = -6;

	float pause_title_margin_x = 15;
	float pause_title_margin_y = -5;

	UI* pause_title = screen.CreateElement(A_mesh_menu_pause_title);
	pause_title->width  = pause_menu->width;
	pause_title->height = pause_menu->width;
	pause_title->x = pause_menu->x - pause_menu->width  + pause_title_margin_x;
	pause_title->y = pause_menu->y + pause_menu->height + pause_title_margin_y;
	pause_title->z = -7;

	screen.Draw(nullptr, Renderer);
}
