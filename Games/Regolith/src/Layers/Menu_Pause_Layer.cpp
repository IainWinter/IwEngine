#include "Layers/Menu_Pause_Layer.h"

int Menu_Pause_Layer::Initialize()
{
	iw::Mesh title  = A_font_cambria->GenerateMesh("Pause Menu", 2);
	iw::Mesh button = A_font_cambria->GenerateMesh("Button", 8);


	m_entity_screen = Space->CreateEntity<UIScreen>();

	m_screen = m_entity_screen.Set<UIScreen>();

	m_background = m_screen->CreateElement(A_mesh_menu_background);
	m_pause_menu  = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title = m_screen->CreateElement(title);
	
	iw::Mesh buttonbg = A_mesh_menu_background.MakeInstance();
	buttonbg.Material->Set("color", iw::Color(.7, .2, .2));

	m_button_test1 = m_screen->CreateElement<UI_Button>(buttonbg, A_font_cambria->GenerateMesh("Button",         12));
	m_button_test2 = m_screen->CreateElement<UI_Button>(buttonbg, A_font_cambria->GenerateMesh("Button 20",      12));
	m_button_test3 = m_screen->CreateElement<UI_Button>(buttonbg, A_font_cambria->GenerateMesh("Button 300",     12));
	m_button_test4 = m_screen->CreateElement<UI_Button>(buttonbg, A_font_cambria->GenerateMesh("Button 400 abc", 12));

	return 0;
}

void Menu_Pause_Layer::Destroy()
{
	m_entity_screen.Destroy();
}

void Menu_Pause_Layer::OnPush()
{
	m_entity_screen.Revive();
}

void Menu_Pause_Layer::OnPop()
{
	m_entity_screen.Kill();
}

void Menu_Pause_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background->width  = m_screen->width;
	m_background->height = m_screen->height;
	m_background->zIndex = -5;

	m_pause_menu->height = 600;
	m_pause_menu->width  = 600;
	m_pause_menu->zIndex = -4;

	m_pause_title->height = 600;
	m_pause_title->width  = 600;
	m_pause_title->x = m_pause_menu->x - m_pause_menu->width  + 15;
	m_pause_title->y = m_pause_menu->y + m_pause_menu->height - 5;
	m_pause_title->zIndex = 5;

	int i = 0;
	for (UI_Button* button : { m_button_test1, m_button_test2, m_button_test3, m_button_test4 })
	{
		button->width = 150;
		button->height = 50;
		button->x = m_pause_menu->x + m_pause_menu->width  - button->width - 15;
		button->y = m_pause_menu->y - m_pause_menu->height + (button->height + 15) * i * 2;
		button->zIndex = 0;

		float buttonOffsetTarget = 0.f;
		if (button->IsPointOver(m_screen->LocalMouse()))
		{
			buttonOffsetTarget = 30;
		}

		button->offset = iw::lerp(button->offset, buttonOffsetTarget, iw::DeltaTime() * 10);
		button->x += button->offset;
		
		i += 1;
	}

}
