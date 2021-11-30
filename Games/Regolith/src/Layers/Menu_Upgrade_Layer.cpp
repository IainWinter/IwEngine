#include "Layers/Menu_Upgrade_Layer.h"

#define LoadTexture Asset->Load<iw::Texture>

UI_Base* Menu_Upgrade_Layer::MakeUpgrade(
	const UpgradeDescription& upgrade)
{
	auto texture = LoadTexture(upgrade.texturePath);
	texture->SetFilter(iw::NEAREST);

	UI_Button* button = m_upgrades->GetRow(0)[0]->CreateElement<UI_Button>();
	button->CreateElement<UI_Image>(texture);

	button->whileMouseHover = [this, button, upgrade]()
	{
		button->offsetTarget = 8;
		m_tooltip->active = true;
		m_tooltip->GetElement<UI_Text>(0)->SetString(upgrade.texturePath);
	};

	return button;
}

int Menu_Upgrade_Layer::Initialize()
{
	m_background = m_screen->CreateElement(A_mesh_highscore_menu_background);

	iw::Mesh itembg = A_mesh_menu_background.MakeInstance();
	itembg.Material->Set("color", iw::Color(.4, .4, .4));

	// if font is monospace, this should be able to just be one element
	// problly going to need to have each be seperate though for scrolling

	m_upgrades = m_screen->CreateElement<Upgrade_Table>(/*itembg*/);
	m_upgrades->fontConfig.Anchor = iw::FontAnchor::TOP_LEFT;

#define _ true,
#define o false,

	m_upgrades->AddRow(1, 2, 3, 4, 5, { o o o _ _ });
	m_upgrades->AddRow(1, 2, 3, 4, 5, { o o o o _ });
	m_upgrades->AddRow(1, 2, 3, 4, 5, { o o o o o });
	m_upgrades->AddRow(1, 2, 3, 4, 5, { o _ _ _ _ });

	UpgradeDescription upgrade1;
	UpgradeDescription upgrade2;
	UpgradeDescription upgrade3;
	UpgradeDescription upgrade4;
	UpgradeDescription upgrade5;
	upgrade1.texturePath = "textures/SpaceGame/upgrade_thrusterimpulse.png";
	upgrade2.texturePath = "textures/SpaceGame/upgrade_thrusteracceleration.png";
	upgrade3.texturePath = "textures/SpaceGame/upgrade_thrustermaxspeed.png";
	upgrade4.texturePath = "textures/SpaceGame/upgrade_healthefficiency.png";
	upgrade5.texturePath = "textures/SpaceGame/upgrade_laserefficiency.png";

	m_upgrades->GetRow(0)[0]->AddElement(MakeUpgrade(upgrade1));
	m_upgrades->GetRow(1)[0]->AddElement(MakeUpgrade(upgrade2));
	m_upgrades->GetRow(2)[0]->AddElement(MakeUpgrade(upgrade3));
	m_upgrades->GetRow(2)[1]->AddElement(MakeUpgrade(upgrade4));
	m_upgrades->GetRow(2)[2]->AddElement(MakeUpgrade(upgrade5));

	m_tooltip = m_screen->CreateElement(itembg);
	UI_Text* text = m_tooltip->CreateElement<UI_Text>(
		A_font_cambria, 
		iw::FontMeshConfig { 30, iw::FontAnchor::TOP_LEFT }
	);

	m_background->zIndex = 0;
	m_upgrades  ->zIndex = -1;
	m_tooltip   ->zIndex = 3;
	text        ->zIndex = 4;

	return 0;
}
//
//float r(float x)
//{
//	return x + abs((fmod(x, 4)) - 4);
//}

void Menu_Upgrade_Layer::PostUpdate()
{
	m_screen->width  = Renderer->Width();
	m_screen->height = Renderer->Height();

	m_background->width  = m_screen->height * .8;
	m_background->height = m_screen->height;

	// could round table size to pixel grid;

	m_upgrades->height =  m_background->height * .64f;
	m_upgrades->width  =  m_background->width  * .85f;
	m_upgrades->y      = -m_background->height * .2f - 4;
	m_upgrades->x      = 8;

	m_upgrades->rowHeight = m_upgrades->height / 5.f;
	for (float& width : m_upgrades->colWidth)
	{
		width = m_upgrades->rowHeight;
	}

	float p = m_background->width / 200.f * 2;

	m_upgrades->rowPadding = p;
	m_upgrades->colPadding = { p, p, p, p, p };

	if (m_tooltip->active)
	{
		iw::vec2 mouse = m_screen->LocalMouse();
		m_tooltip->width = 200;
		m_tooltip->height = 100;
		m_tooltip->x = mouse.x() + m_tooltip->width;
		m_tooltip->y = mouse.y() - m_tooltip->height;

		m_tooltip->GetElement(0)->width  = 200;
		m_tooltip->GetElement(0)->height = 200;

	}

	m_tooltip->active = false;

	ButtonUpdate();
}
