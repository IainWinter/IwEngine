#include "Layers/Menu_Pause_Layer.h"

int Menu_Pause_Layer::Initialize()
{
	iw::Mesh title = A_font_cambria->GenerateMesh("Pause Menu", { 48 });

	m_pause_menu  = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title = m_screen->CreateElement(title);

	iw::Mesh buttonbg = A_mesh_menu_background.MakeInstance();
	buttonbg.Material->Set("color", iw::Color(.7, .2, .2));

	AddButton(buttonbg, A_font_cambria->GenerateMesh("Resume", { 360 }));
	m_buttons.at(0)->onClick = [&]()
	{
		Console->QueueCommand("escape");
	};

	//AddButton(buttonbg, A_font_cambria->GenerateMesh("Gameover", 16));
	//m_buttons.at(1)->onClick = [&]()
	//{
	//	Console->QueueCommand("game-over");
	//};

	return 0;
}

void Menu_Pause_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_pause_menu ->zIndex = -1;
	m_pause_title->zIndex = 0;

	m_pause_menu->height = 600;
	m_pause_menu->width  = 600;
	m_pause_menu->y = m_screen->height * .2;

	m_pause_title->height = 600;
	m_pause_title->width  = 600;
	m_pause_title->x = m_pause_menu->x - m_pause_menu->width  + 15;
	m_pause_title->y = m_pause_menu->y + m_pause_menu->height - 5;

	// use ButtonUpdate

	int i = 0;
	for (UI_Button* button : m_buttons)
	{
		button->zIndex = 1;

		button->width = 150;
		button->height = 40;
		button->x = m_pause_menu->x - m_pause_menu->width  +  button->width  + 15;
		button->y = m_pause_menu->y + m_pause_menu->height - (button->height + 15) * i * 2 - 200;

		float buttonOffsetTarget = 0.f;
		if (button->IsPointOver(m_screen->LocalMouse()))
		{
			buttonOffsetTarget = 30;

			if (m_execute)
			{
				buttonOffsetTarget = 0;
			}

			else 
			if (   m_last_execute
				&& button->onClick)
			{
				button->onClick();
			}
		}

		button->offset = iw::lerp(button->offset, buttonOffsetTarget, iw::DeltaTime() * 20);
		button->x += floor(button->offset);
		
		i += 1;
	}

	m_last_execute = m_execute;
}
