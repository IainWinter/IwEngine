#include "Layers/Menu_PostGame_Layer.h"

int Menu_PostGame_Layer::Initialize()
{
	iw::Mesh finalScore = A_font_cambria->GenerateMesh("FINAL SCORE", {2, iw::FontAnchor::TOP_LEFT}); // should be centered
	//iw::Mesh final_score = A_font_cambria->GenerateMesh("Final score", 1);
	iw::Mesh label_restart = A_font_cambria->GenerateMesh("REFORM", { 16 });
	iw::Mesh label_quit    = A_font_cambria->GenerateMesh("QUIT", { 16 });

	iw::Mesh buttonbg = A_mesh_menu_background.MakeInstance();
	buttonbg.Material->Set("color", iw::Color(.2, .2, .2));

	//A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1)); // maybe this should be the full player

	auto [min, max] = finalScore.GetBounds<iw::d2>();
	m_title_FinalScoreWidth = max.x - min.x;

	m_background   = m_screen->CreateElement(A_mesh_menu_background);
	m_playerBorder = m_screen->CreateElement(A_mesh_ui_playerBorder);

	m_title_finalScore = m_screen->CreateElement(finalScore);

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

	return 0;
}

void Menu_PostGame_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background ->zIndex = -1;
	m_background->width  = m_screen->height * .8;
	m_background->height = m_screen->height;
	
	m_playerBorder->height = m_background->height * .3f;
	m_playerBorder->width  = m_playerBorder->height; // 1:1 ratio
	m_playerBorder->y = m_background->height * .5;

	m_title_finalScore->height = m_background->width;
	m_title_finalScore->width  = m_background->width;

	/*
	m_title->width  =  m_background->width;
	m_title->height =  m_background->width;
	m_title->x      = -m_background->width  + 35;
	m_title->y      =  m_background->height - 35;

	m_score_title->width  =  m_background->width;
	m_score_title->height =  m_background->width;
	m_score_title->x      =  m_title->x;
	m_score_title->y      =  m_title->y - m_background->height * .15;

	m_score->width  = m_background->width;
	m_score->height = m_background->width;
	m_score->x      = m_title->x;
	m_score->y      = m_score_title->y - m_background->height * .05;

	m_player->width  = m_background->width * .25;
	m_player->height = m_player->width;
	m_player->x = m_background->x + m_background->width  * .25;
	m_player->y = m_background->y + m_background->height * .25;
		
		*/

	float buttonOffsetTarget = 0;
	if (m_button_reform->IsPointOver(m_screen->LocalMouse()))
	{
		buttonOffsetTarget = 10;

		if (m_execute)
		{
			buttonOffsetTarget = 0;
		}

		else
		if (   m_last_execute
			&& m_button_reform->onClick)
		{
			m_button_reform->onClick();
		}
	}


	m_button_reform->offset = iw::lerp(m_button_reform->offset, buttonOffsetTarget, iw::DeltaTime() * 20);

	m_button_reform->width = m_background->width * .18f;
	m_button_reform->height = m_button_reform->width / 3;
	m_button_reform->x = m_background->x - m_background->width * .25;;
	m_button_reform->y = m_background->y + floor(m_button_reform->offset);
}
