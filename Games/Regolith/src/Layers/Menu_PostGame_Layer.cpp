#include "Layers/Menu_PostGame_Layer.h"

int Menu_PostGame_Layer::Initialize()
{
	// font seems to change height based on what letters there are I is bigger than others,
	// font should be monospaced though???

	iw::Mesh title_score   = A_font_cambria->GenerateMesh("SCORE",  { 36, iw::FontAnchor::TOP_CENTER });
	iw::Mesh label_restart = A_font_cambria->GenerateMesh("REFORM", { 300 });
	iw::Mesh label_quit    = A_font_cambria->GenerateMesh("QUIT",   { 300 });

	iw::Mesh buttonbg = A_mesh_menu_background.MakeInstance();
	buttonbg.Material->Set("color", iw::Color(.2, .2, .2));

	iw::Mesh tablebg = A_mesh_menu_background.MakeInstance();
	tablebg.Material->Set("color", iw::Color(.4, .4, .4));

	mat_tableItem = A_mesh_menu_background.Material->MakeInstance();
	mat_tableItem ->Set("useBoxFade", 1.0f);
	mat_tableItem ->Set("fade", 10000.f);

	mat_tableText = A_font_cambria->m_material->MakeInstance();
	mat_tableText->Set("useBoxFade", 1.0f);
	mat_tableText->Set("fade", 10000.f);

	iw::Mesh tableitembg = A_mesh_menu_background.MakeInstance();
	tableitembg.Material = mat_tableItem;

	// if font is monospace, this should be able to just be one element
	// problly going to need to have each be seperate though for scrolling

	m_table_highScore = m_screen->CreateElement<Score_Table>(tablebg, tableitembg, A_font_cambria);
	m_table_highScore->fontConfig.Material = mat_tableText;

	m_table_highScore->sort = [](
		const Score_Table::data_t& a,
		const Score_Table::data_t& b)
	{
		return std::get<1>(a) < std::get<1>(b);
	};

	m_table_highScore->rowHeight = 50.f;
	m_table_highScore->rowPadding = 10.f;
	m_table_highScore->colPadding = { 10, 10, 10 };

	////A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1)); // maybe this should be the full player

	m_background   = m_screen->CreateElement(A_mesh_menu_background);
	m_playerBorder = m_screen->CreateElement(A_mesh_ui_playerBorder);

	m_title_score     = m_screen->CreateElement(title_score);
	//m_table_highScore = m_screen->CreateElement(highscoreTable);

	m_button_reform = m_screen->CreateElement<UI_Button>(buttonbg, label_restart);
	m_button_reform->onClick = [&]()
	{
		SubmitScoreAndExit("game-start");
	};

	m_button_quit   = m_screen->CreateElement<UI_Button>(buttonbg, label_quit);
	m_button_quit->onClick = [&]()
	{
		SubmitScoreAndExit("quit");
	};

	m_background     ->zIndex = 0;
	m_table_highScore->zIndex = 3;
	m_playerBorder   ->zIndex = 1;
	m_title_score    ->zIndex = 1;
	m_button_reform  ->zIndex = 1;
	m_button_quit    ->zIndex = 1;

	// add final score to highscore table and set selected 

	HighscoreRecord playerRecord;
	playerRecord.Name = "Anon"; // could return a number of players ie Anon10902
	playerRecord.Score = m_finalScore;
	playerRecord.Order = 0; // need to find from web

	m_playerRowId = AddHighscoreToTable(playerRecord, true);
	mat_tablePlayerRow = m_table_highScore->GetRow(m_playerRowId)->at(0)->mesh.Material;

	// load highscores

	iw::HttpRequest<iw::JsonSerializer> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.winter.dev";
	request.Resource = "/regolith/php/get_highscores.php";

	m_scores = m_connection.AsyncRequest<std::vector<HighscoreRecord>>(request);

	return 0;
}

void Menu_PostGame_Layer::PostUpdate()
{
	if (m_scores.HasValue())
	{
		for (HighscoreRecord& record : m_scores.Value())
		{
			AddHighscoreToTable(record);
		}

		m_scores.DoneWithValue();
	}

	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background->width  = m_screen->height * .8;
	m_background->height = m_screen->height;
	
	float paddingX = .05 * m_background->width;
	float paddingY = paddingX;

	m_playerBorder->height = m_background->height * .3f;
	m_playerBorder->width  = m_playerBorder->height; // 1:1 ratio
	m_playerBorder->y = m_background->height * .5;

	m_title_score->width  = m_background->width;
	m_title_score->height = m_background->width;
	m_title_score->y = m_playerBorder->y - m_playerBorder->height - paddingY;

	m_table_highScore->height = m_background->height * .4f;
	m_table_highScore->width  = m_background->width  * .8f;
	m_table_highScore->y      = -m_table_highScore->height;

	m_table_highScore->colWidth[2] = (m_table_highScore->width) * .5;
	m_table_highScore->colWidth[1] = (m_table_highScore->width - 260.f) * .5;
	m_table_highScore->colWidth[0] = m_table_highScore->WidthRemaining(0);

	// highlight player row
	float rgb = (sin(iw::TotalTime() * 2.f) + 1.1) / 4.f; // sin wave from 0 to .5
	mat_tablePlayerRow->Set("color", iw::Color(rgb, rgb, rgb, 1.f));

	// Button button menu (reform / quit)

	float button_x = m_background->width;

	for (UI_Button* button : { m_button_quit, m_button_reform })
	{
		button->width  = m_background->width * .18f;
		button->height = button->width / 3;
		button->y = -m_background->height + button->height + paddingY;

		button->x = button_x - button->width - paddingX;

		button_x -= button->width * 2 + paddingX / 4;
	}

	ButtonUpdate();

	// box size in screen space

	float minX = (m_table_highScore->x - m_table_highScore->width)                                  / m_screen->width; // min/max x
	float maxX = (m_table_highScore->x + m_table_highScore->width)                                  / m_screen->width;
	float minY = (m_table_highScore->y - m_table_highScore->height + m_table_highScore->rowPadding) / m_screen->height; // min/max y
	float maxY = (m_table_highScore->y + m_table_highScore->height - m_table_highScore->rowPadding) / m_screen->height;

	// I dont think this paddin should be *2 but it works

	SetTableBoxFade(minX, minY, maxX, maxY);
}

bool Menu_PostGame_Layer::On(
	iw::KeyTypedEvent& e)
{
	if (m_playerRowId >= 0)
	{
		bool deleteChar = m_playerName.size() > 16;

		switch (e.Character)
		{
			case '\b':
			{
				deleteChar = true;
				break;
			}
		}

		if (deleteChar)
		{
			m_playerName = m_playerName.substr(0, m_playerName.size() - 1);
		}

		else
		if (   (e.Character >= 'A' && e.Character <= 'Z')
			|| (e.Character >= 'a' && e.Character <= 'z')
			|| (e.Character >= '0' && e.Character <= '9')
			|| (e.Character == ' ' && m_playerName.size() > 0 && m_playerName[m_playerName.size() - 1] != ' ')
/*			|| (e.Character == '-')
			|| (e.Character == '_')
			|| (e.Character == '=')
			|| (e.Character == '+')
			|| (e.Character == '*')*/)
		{
			m_playerName += e.Character;
		}

		m_table_highScore->UpdateRow(m_playerRowId, 2, m_playerName);
	}

	return false;
}

bool Menu_PostGame_Layer::On(iw::MouseWheelEvent& e)
{
	m_table_highScore->scrollOffset -= e.Delta * 20;
	return false;
}

int Menu_PostGame_Layer::AddHighscoreToTable(
	const HighscoreRecord& record,
	bool makeInstanceOfBackgroundMesh)
{
	// insert into table in correct order

	return m_table_highScore->AddRow(
		record.Order,
		record.Score,
		record.Name,
		makeInstanceOfBackgroundMesh
	);
}

void Menu_PostGame_Layer::SubmitScoreAndExit(
	const std::string& whereTo)
{
	//Console->QueueCommand(whereTo);

	iw::HttpRequest<iw::None> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.winter.dev";
	request.Resource = "/regolith/php/submit_highscore.php";

	request.SetArgument("name", m_playerName);
	request.SetArgument("score", to_string(m_finalScore));

	m_connection.Request<std::string>(request, [](
		std::string& str) 
	{
		LOG_INFO << str;
	});
}

void Menu_PostGame_Layer::SetTableBoxFade(
	float minX, float minY, 
	float maxX, float maxY)
{
	glm::vec4 box = glm::vec4(minX, minY, maxX, maxY);
	mat_tableItem->Set("box", box);
	mat_tableText->Set("box", box);
	mat_tablePlayerRow->Set("box", box);
}
