#include "Layers/Menu_Pause_Layer.h"

int Menu_Pause_Layer::Initialize()
{
	iw::Mesh title = A_font_cambria->GenerateMesh("Pause Menu", { 48 });
	iw::Mesh label_resume = A_font_cambria->GenerateMesh("Resume", { 360 });

	m_pause_menu  = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title = m_screen->CreateElement(title);

	/*m_button_resume = m_screen->CreateElement<UI_Button>(
		m(.7, .2, .2), label_resume);

	m_button_resume->onClick = [&]()
	{
		Console->QueueCommand("escape");
	};*/

	m_slider_volume = m_screen->CreateElement<UI_Slider>(
		m(.5, .5, .5), ma(.1, .1, 1, .75), "Volume", A_font_cambria);

	m_slider_volume->onChangeValue = [this](
		float value)
	{
		Audio->SetVolume(value);
	};

	//m_items.push_back(m_button_resume);
	m_items.push_back(m_slider_volume);

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

	m_slider_volume->width  = 300;
	m_slider_volume->height = 40;
	m_slider_volume->zIndex = 1;

	//m_button_resume->width  = 150;
	//m_button_resume->height = 40;

	int i = 0;
	for (UI* item : m_items)
	{
		item->zIndex = 1;

		item->x = m_pause_menu->x - m_pause_menu->width  +  item->width  + 15;
		item->y = m_pause_menu->y + m_pause_menu->height - (item->height + 15) * i * 2 - 200;
		
		i += 1;
	}

	ButtonUpdate();
}
