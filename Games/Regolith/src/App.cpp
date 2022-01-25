#include "App.h"
#include <combaseapi.h>

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

	m_menus = new Menu_Title_Layer();

	Console->QueueCommand("set-state menus");

	// add default cells

	iw::Cell c;
	c.Type        = iw::CellType::LIGHTNING;
	c.Props       = iw::CellProp::NONE;
	c.Style       = iw::CellStyle::SHIMMER;
	c.StyleColor  = iw::Color(.1, .1, .1, 0);
	c.Color       = iw::Color::From255(212, 194, 252);
	c.life        = .1;
	
	iw::Cell::SetDefault(iw::CellType::LIGHTNING, c);

	//m_gamePause = new GameState("Pause menu", StateName::GAME_PAUSE_STATE);
	//m_gamePause->Layers.push_back(new Menu_Pause_Layer());
	//m_gamePause->OnChange = [&]()
	//{
	//	Physics->Paused = true;
	//	//Window()->SetCursor(true);
	//	//Renderer->Device->SetVSync();
	//	Input->SetContext("Menu");
	//};

	//Console->QueueCommand("set-state title");

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

	iw::ref<iw::Context> context_game = Input->CreateContext("game");
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

	iw::ref<iw::Context> context_menu = Input->CreateContext("menu");
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

	// todo: all the m_menus-> calls should be handled by the manu layer itself
	//		 all the Input-> calls should be handled in a event listener for state changes

	Console->AddHandler([&](
		const iw::Command& command)
	{
		LOG_INFO << command.Original;

		if (command.Verb == "escape")
		{
			switch (m_state)
			{
				case StateName::IN_GAME:
					ChangeToPauseMenu();
					break;
				case StateName::IN_GAME_THEN_MENU:
					ChangeBackToGame();
					break;
				default:
					break;
			}
		}

		//else
		//if (command.Verb == "fade")
		//{
		//	float fadeTime  = command.TokenCount > 0 ? command.Tokens[0].Float : 1.f;
		//	float fadeDelay = command.TokenCount > 1 ? command.Tokens[1].Float : 0.f;
		//	float fadeTotalTime = fadeTime + fadeDelay;

		//	iw::Layer* fadeLayer = PushLayer<Menu_Fadeout_Layer>(fadeTime, fadeDelay);

		//	float time = iw::RawTotalTime();
		//	Task->coroutine(
		//		[this, time, fadeTotalTime, fadeLayer]()
		//	{
		//		bool done = iw::RawTotalTime() - time > fadeTotalTime;
		//		if (done) PopLayer(fadeLayer);
		//		return done;
		//	});
		//}

		else
		if (command.Verb == "set-state")
		{
			std::string name  = command.Str(0);
			float       delay = command.Num(1);

			float time = iw::TotalTime();
			Task->coroutine(
				[=]()
			{
				bool finished = iw::TotalTime() - time > delay;
				if (finished)
				{
					SetState(name);
				}

				return finished;
			});
		}

		//else
		//if (command.Verb == "push-state")
		//{
		//	std::string stateName = command.Tokens[0].String;

		//	     if (stateName == "settings")   PushState(MakeState_Settings());
		//	else if (stateName == "highscores") PushState(MakeState_Highscores());
		//	//else if (stateName == "settings") PushState(MakeState_Settings());

		//	else
		//	{
		//		LOG_ERROR << "[push-state] invalid state";
		//	}
		//}

		//else
		//if (command.Verb == "push-state")
		//{
		//	PopState();
		//}

		else
		if (command.Verb == "final-score")
		{
			//m_finalScore = command.Tokens[0].Int;
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
