#include "Layers/Menu_PostGame_Layer.h"

int Menu_PostGame_Layer::Initialize()
{
	iw::Mesh title = A_font_cambria->GenerateMesh("You have been destroyed", 2);
	iw::Mesh final_score = A_font_cambria->GenerateMesh("Final score", 1);
	iw::Mesh restart_label = A_font_cambria->GenerateMesh("Restart", 16);

	iw::Mesh buttonbg = A_mesh_menu_background.MakeInstance();
	buttonbg.Material->Set("color", iw::Color(.2, .2, .2));

	A_mesh_ui_playerHealth.Material->Set("color", iw::Color(1));

	m_background  = m_screen->CreateElement(A_mesh_menu_background);
	m_title       = m_screen->CreateElement(title);
	m_score_title = m_screen->CreateElement(final_score);
	m_score       = m_screen->CreateElement(A_mesh_ui_text_score);
	m_player      = m_screen->CreateElement(A_mesh_ui_playerHealth);
	m_restart     = m_screen->CreateElement<UI_Button>(buttonbg, restart_label);

	m_restart->onClick = [&]()
	{
		Console->QueueCommand("game-start");
	};

	return 0;
}

void Menu_PostGame_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background ->zIndex = -1;

	m_background->width  = (m_screen->height * .8);
	m_background->height = m_screen->height;

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

	float buttonOffsetTarget = 0;
	if (m_restart->IsPointOver(m_screen->LocalMouse()))
	{
		buttonOffsetTarget = 10;

		if (m_execute)
		{
			buttonOffsetTarget = 0;
		}

		else
		if (   m_last_execute
			&& m_restart->onClick)
		{
			m_restart->onClick();
		}
	}

	m_restart->offset = iw::lerp(m_restart->offset, buttonOffsetTarget, iw::DeltaTime() * 20);

	m_restart->width = m_background->width * .18f;
	m_restart->height = m_restart->width / 3;
	m_restart->x = m_background->x - m_background->width * .25;;
	m_restart->y = m_background->y + floor(m_restart->offset);

	m_last_execute = m_execute;
}
