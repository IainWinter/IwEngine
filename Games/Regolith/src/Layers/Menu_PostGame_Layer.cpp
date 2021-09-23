#include "Layers/Menu_PostGame_Layer.h"

//#include "iw/common/algos/polygon2.h"

//struct ButtonUI : UI
//{
//	ButtonUI(
//		const iw::Mesh& mesh
//	) 
//		: UI(mesh)
//	{}
//
//	
//};

int Menu_PostGame_Layer::Initialize()
{
	m_screen = Space->CreateEntity<UI_Screen>().Set<UI_Screen>();
	m_background  = m_screen->CreateElement(A_mesh_menu_background);
	m_pause_menu  = m_screen->CreateElement(A_mesh_menu_pause);
	m_pause_title = m_screen->CreateElement(A_mesh_menu_pause);

	return 0;
}

void Menu_PostGame_Layer::Destroy()
{
	Space->FindEntity(m_screen).Destroy();
}

void Menu_PostGame_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width()/4;
	m_screen->height = Renderer->Height()/4;

	m_background->width  = m_screen->width;
	m_background->height = m_screen->height;
	m_background->zIndex = -5;

	m_pause_menu->height = 600;
	m_pause_menu->width  = 600;
	m_pause_menu->zIndex = -6;

	m_pause_title->width  = m_pause_menu->width;
	m_pause_title->height = m_pause_menu->width;
	m_pause_title->x = m_pause_menu->x - m_pause_menu->width  + 15;
	m_pause_title->y = m_pause_menu->y + m_pause_menu->height - 5;
	m_pause_title->zIndex = -7;
}
