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

	iw::Mesh tableitembg = A_mesh_menu_background.MakeInstance();
	tableitembg.Material->Set("color", iw::Color(.1, .1, .1));

	// if font is monospace, this should be able to just be one element
	// problly going to need to have each be seperate though for scrolling

	m_table_highScore = m_screen->CreateElement<UI_Table>(tablebg, tableitembg, A_font_cambria);
	
	m_table_highScore->rowHeight = 50.f;
	m_table_highScore->rowPadding = 10.f;
	m_table_highScore->colPadding = { 10.f, 10.f, 0.f };

	////A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1)); // maybe this should be the full player

	m_background   = m_screen->CreateElement(A_mesh_menu_background);
	m_playerBorder = m_screen->CreateElement(A_mesh_ui_playerBorder);

	m_title_score     = m_screen->CreateElement(title_score);
	//m_table_highScore = m_screen->CreateElement(highscoreTable);

	m_button_reform = m_screen->CreateElement<UI_Button>(buttonbg, label_restart);
	m_button_reform->onClick = [&]()
	{
		Console->QueueCommand("game-start");
	};

	m_button_quit   = m_screen->CreateElement<UI_Button>(buttonbg, label_quit);
	m_button_quit->onClick = [&]()
	{
		Console->QueueCommand("quit");
	};

	m_background     ->zIndex = -2;
	m_table_highScore->zIndex = 0;

	// load highscores

	m_scores = m_connection.AsyncRequest<std::vector<HighscoreRecord>, iw::JsonSerializer>();

	return 0;
}

void Menu_PostGame_Layer::PostUpdate()
{
	if (m_scores.HasValue())
	{
		for (HighscoreRecord& record : m_scores.Value())
		{
			std::stringstream score; score << record.Score;
			std::stringstream order; order << record.Order;
			m_table_highScore->AddRow({ record.Name, score.str(), order.str() });
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

	m_table_highScore->colWidth[0] = (m_table_highScore->width - 130.f) * .5;
	m_table_highScore->colWidth[1] = (m_table_highScore->width - 130.f) * .5;
	m_table_highScore->colWidth[2] = 75.f; // should calc based on size remaining
	
	m_table_highScore->colPadding[2] = 80.f;

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
}
