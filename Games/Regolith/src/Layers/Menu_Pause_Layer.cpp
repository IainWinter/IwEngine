#include "Layers/Menu_Pause_Layer.h"

int Menu_Pause_Layer::Initialize()
{
	iw::Mesh title = A_font_cambria->GenerateMesh("Pause Menu", { 48 });
	iw::Mesh volume_title = A_font_cambria->GenerateMesh("Volume", { 360 });

	m_pause_menu   = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title  = m_screen->CreateElement(title);
	m_volume_title = m_screen->CreateElement(volume_title);

	m_button_resume = m_screen->CreateElement<UI_Button>(m(.7, .2, .2));
	m_button_resume->CreateElement<UI_Text>("Resume", iw::FontMeshConfig{ 120 });
	m_button_resume->onClick = [&]()
	{
		Console->QueueCommand("escape");
	};

	int musicHandle  = Audio->GetHandle("vca:/music");
	int effectHandle = Audio->GetHandle("vca:/effects");

	float initalMusic  = 0;
	float initalEffect = 0;
	
	Audio->GetVolume(musicHandle, initalMusic);
	Audio->GetVolume(effectHandle, initalEffect);

	m_slider_music = m_screen->CreateElement<UI_Slider>(m(.5, .5, .5), ma(.1, .1, 1, .75), "Music", A_font_cambria);
	m_slider_music->onChangeValue = [this, musicHandle](
		float value)
	{
		Audio->SetVolume(musicHandle, value);
	};
	m_slider_music->UpdateValue(initalMusic);

	m_slider_effect = m_screen->CreateElement<UI_Slider>(m(.5, .5, .5), ma(.1, .1, 1, .75), "Effects", A_font_cambria);
	m_slider_effect->onChangeValue = [this, effectHandle](
		float value)
	{
		Audio->SetVolume(effectHandle, value);
	};
	m_slider_effect->UpdateValue(initalEffect);

	m_items.push_back(m_button_resume);
	m_items.push_back(m_volume_title);
	m_items.push_back(m_slider_music);
	m_items.push_back(m_slider_effect);

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

	m_volume_title->height = 40;
	m_volume_title->width  = 40;

	m_slider_music->width  = 300;
	m_slider_music->height = 40;
	m_slider_music->zIndex = 1;

	m_slider_effect->width = 300;
	m_slider_effect->height = 40;
	m_slider_effect->zIndex = 1;

	m_button_resume->width  = 150;
	m_button_resume->height = 40;

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
