#include "Layers/Menu_Upgrade_Layer.h"

#define LoadTexture(x) Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)
#define LoadFont(x) Asset->Load<iw::Font>(std::string("fonts/") + x)

UI_Base* Menu_Upgrade_Layer::MakeUpgrade(
	const UpgradeDescription& upgrade)
{
	UI_Button* button = m_upgrades->GetRow(0)[0]->CreateElement<UI_Button>();
	button->CreateElement<UI_Image>(LoadTexture(upgrade.texturePath));

	button->whileMouseHover = [this, button, upgrade]()
	{
		button->offsetTarget = 8;
		m_tooltip->active = true;
		m_tooltip->GetElement<UI_Text>(0)->SetString(upgrade.tooltip);
		m_tooltip->GetElement<UI_Text>(1)->SetString(upgrade.description);
		m_tooltip->GetElement<UI_Text>(2)->SetString(upgrade.stat);
		m_tooltip->GetElement<UI_Text>(3)->SetString(upgrade.cost);

		m_tooltip->GetElement<UI_Text>(2)->mesh.Material->Set("color", upgrade.statColor);
		m_tooltip->GetElement<UI_Text>(3)->mesh.Material->Set("color", upgrade.costColor);
	};

	button->onClick = [button]()
	{
		button->active = false;
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

	UpgradeDescription upgrade1(true);
	UpgradeDescription upgrade2(true);
	UpgradeDescription upgrade3(true);
	UpgradeDescription upgrade4(true);
	UpgradeDescription upgrade5(true);
	upgrade1.texturePath = "upgrade_thrusterimpulse.png";
	upgrade2.texturePath = "upgrade_thrusteracceleration.png";
	upgrade3.texturePath = "upgrade_thrustermaxspeed.png";
	upgrade4.texturePath = "upgrade_healthefficiency.png";
	upgrade5.texturePath = "upgrade_laserefficiency.png";

	upgrade1.tooltip = "Thruster\nImpulse";
	upgrade1.description = "Effects the immediate stop \nand start time of \nthe ship";
	upgrade1.stat = "+10%";
	upgrade1.cost = "200 R";

	upgrade2.tooltip = "Thruster\nAcceleration";
	upgrade2.description = "Effects the time to\nreach max speed";
	upgrade2.stat = "+10%";
	upgrade2.cost = "1,000 R";

	upgrade3.tooltip = "Thruster\nMax Velocity";
	upgrade3.description = "Effects how fast the\nship can move";
	upgrade3.stat = "+10%";
	upgrade3.cost = "2,000 R";

	upgrade4.tooltip = "Health Pickup\nEfficiency";
	upgrade4.description = "Effects how many cells get\nrestored per green pickup";
	upgrade4.stat = "+2 cells";
	upgrade4.cost = "2,000 R";

	upgrade5.tooltip = "Laser Pickup\nEfficiency";
	upgrade5.description = "Effects how much liquid get\nconverted per green pickup";
	upgrade5.stat = "+2 charges";
	upgrade5.cost = "3,000 R";

	m_upgrades->GetRow(0)[0]->AddElement(MakeUpgrade(upgrade1));
	m_upgrades->GetRow(1)[0]->AddElement(MakeUpgrade(upgrade2));
	m_upgrades->GetRow(2)[0]->AddElement(MakeUpgrade(upgrade3));
	m_upgrades->GetRow(2)[1]->AddElement(MakeUpgrade(upgrade4));
	m_upgrades->GetRow(2)[2]->AddElement(MakeUpgrade(upgrade5));

	auto font = LoadFont("basic.fnt");
	font->m_material = A_material_font_cam->MakeInstance();
	font->m_material->SetTexture("texture", font->GetTexture(0));
	font->m_material->Set       ("isFont",     1.0f);
	font->m_material->Set       ("font_edge",  0.5f);
	font->m_material->Set       ("font_width", 0.25f);
	font->GetTexture(0)->SetFilter(iw::NEAREST);

	m_tooltip = m_screen->CreateElement<UI_Image>(LoadTexture("ui_upgrade_tooltip_background.png"));
	UI_Text* ttip = m_tooltip->CreateElement<UI_Text>(font, iw::FontMeshConfig { 15, iw::FontAnchor::TOP_LEFT });
	UI_Text* desc = m_tooltip->CreateElement<UI_Text>(font, iw::FontMeshConfig { 10, iw::FontAnchor::TOP_LEFT });
	UI_Text* stat = m_tooltip->CreateElement<UI_Text>(font, iw::FontMeshConfig { 15, iw::FontAnchor::BOT_LEFT });
	UI_Text* cost = m_tooltip->CreateElement<UI_Text>(font, iw::FontMeshConfig { 15, iw::FontAnchor::BOT_RIGHT });

	ttip->zIndex = 4;
	desc->zIndex = 4;
	stat->zIndex = 4;
	cost->zIndex = 4;

	m_background->zIndex =  0;
	m_upgrades  ->zIndex = -1;
	m_tooltip   ->zIndex =  3;

	// test for font rendering
	//for (int i = 1; i < 10; i++)
	//{
	//	UI_Text* text = m_screen->CreateElement<UI_Text>(font, iw::FontMeshConfig { i * 10.f });
	//	text->x = -100;
	//	text->y = -400 + i * 100;
	//	text->width  = 100;
	//	text->height = 100;
	//	text->zIndex = 4;

	//	text->SetString("ABCDEFGHIJKabcdef");
	//}

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
		m_tooltip->width  = m_background->width * 0.5f;
		m_tooltip->height = m_background->height * .26f;
		m_tooltip->x = mouse.x() + m_tooltip->width + 10;
		m_tooltip->y = mouse.y() - m_tooltip->height - 10;

		UI_Text* ttip = m_tooltip->GetElement<UI_Text>(0);
		UI_Text* desc = m_tooltip->GetElement<UI_Text>(1);
		UI_Text* stat = m_tooltip->GetElement<UI_Text>(2);
		UI_Text* cost = m_tooltip->GetElement<UI_Text>(3);

		for (UI_Text* text : { ttip, desc, stat, cost })
		{
			text->width  = m_tooltip->width;
			text->height = m_tooltip->width;
		}

		float padding = .06;

		for (UI_Text* text : { ttip, desc, stat })
		{
			text->x = -m_tooltip->width + m_tooltip->width * padding;
		}

		cost->x = m_tooltip->width - m_tooltip->width * padding * 2;

		ttip->y = m_tooltip->height - m_tooltip->width * padding * 2;
		desc->y = m_tooltip->width * padding * 2;
		stat->y = -m_tooltip->height + m_tooltip->width * padding / 2;
		cost->y = -m_tooltip->height + m_tooltip->width * padding / 2;

		//     o-----------------------------o
		//     |	+10% larger engine		 |
		//     |	impulse				     |
		//     |							 |
		//     |	This effects the initial |
		//     |	ac/deleration of the     |
		//     |	ship				     |
		//     |				   10,200 Rh |
		//     o-----------------------------o
	}

	m_tooltip->active = false;

	ButtonUpdate();
}
