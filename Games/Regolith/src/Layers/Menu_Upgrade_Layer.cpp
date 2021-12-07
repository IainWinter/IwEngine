#include "Layers/Menu_Upgrade_Layer.h"

UI_Base* Menu_Upgrade_Layer::AddUpgrade(
	int x, int y,
	UpgradeDescription* upgrade)
{
	m_upgrades->Row(x)->Data<UpgradeDescription*>(y) = upgrade;

	UI_Button* button = m_upgrades->Row(x)->Elem(y)->CreateElement<UI_Button>();
	button->CreateElement<UI_Image>(LoadTexture(upgrade->TexturePath));

	button->whileMouseHover = [this, button, upgrade]()
	{
		button->offsetTarget = 8;
		m_tooltip->active = true;
		m_tooltip->GetElement<UI_Text>(0)->SetString(upgrade->DispName);
		m_tooltip->GetElement<UI_Text>(1)->SetString(upgrade->Description);
		m_tooltip->GetElement<UI_Text>(2)->SetString(upgrade->Stat);
		m_tooltip->GetElement<UI_Text>(3)->SetString(to_string(upgrade->Cost) + "R");

		m_tooltip->GetElement<UI_Text>(2)->mesh.Material->Set("color", upgrade->StatColor);
		m_tooltip->GetElement<UI_Text>(3)->mesh.Material->Set("color", upgrade->CostColor);
	};

	button->onClick = [this, button, upgrade]()
	{
		button->active = false;

		int row = m_bought->AddRow(upgrade, upgrade->Name);
		UI_Button* xb = m_bought->Row(row)->Elem(0)->SetElement<UI_Button>();
		xb->CreateElement<UI_Image>(LoadTexture("x.png"));

		xb->onClick = [this, button, row, upgrade]()
		{
			button->active = true;

			toActivate.erase(
				std::find(
					toActivate.begin(),
					toActivate.end(),
					m_bought->Row(row)->Data<0>()
				)
			);

			UpdateMoneyText(+upgrade->Cost);
			m_bought->RemoveRow(row);
		};

		UpdateMoneyText(-upgrade->Cost);
		toActivate.push_back(upgrade);
	};

	return button;
}

void Menu_Upgrade_Layer::UpdateMoneyText(
	int diff)
{
	m_money += diff;
	m_text_money->SetString(to_string(m_money) + "R");
}

int Menu_Upgrade_Layer::Initialize()
{
	m_background = m_screen->CreateElement(A_mesh_highscore_menu_background);

	iw::Mesh itembg = A_mesh_menu_background.MakeInstance();
	itembg.Material->Set("color", iw::Color(.4, .4, .4));

	// if font is monospace, this should be able to just be one element
	// problly going to need to have each be seperate though for scrolling

	m_bought = m_screen->CreateElement<Bought_Table>();
	m_bought->background = itembg;
	m_bought->font = m_screen->defaultFont;
	m_bought->fontConfig.Size = 100;

	m_upgrades = m_screen->CreateElement<Upgrade_Table>();

#define _ true,
#define o false,

	m_upgrades->AddRow(0, 0, 0, 0, 0, { o o o _ _ });
	m_upgrades->AddRow(0, 0, 0, 0, 0, { o o o o _ });
	m_upgrades->AddRow(0, 0, 0, 0, 0, { o o o o o });
	m_upgrades->AddRow(0, 0, 0, 0, 0, { o _ _ _ _ });

	AddUpgrade(0, 0, MakeThrusterImpulse());
	AddUpgrade(1, 0, MakeThrusterAcceleration());
	AddUpgrade(2, 0, MakeThrusterMaxSpeed());
	AddUpgrade(2, 1, MakeHealthPickupEfficiency());
	AddUpgrade(2, 2, MakeLaserPickupEfficiency());

	m_tooltip = m_screen->CreateElement<UI_Image>(LoadTexture("ui_upgrade_tooltip_background.png"));
	UI_Text* ttip = m_tooltip->CreateElement<UI_Text>("", iw::FontMeshConfig { 15, iw::TOP_LEFT  });
	UI_Text* desc = m_tooltip->CreateElement<UI_Text>("", iw::FontMeshConfig { 10, iw::TOP_LEFT  });
	UI_Text* stat = m_tooltip->CreateElement<UI_Text>("", iw::FontMeshConfig { 15, iw::BOT_LEFT  });
	UI_Text* cost = m_tooltip->CreateElement<UI_Text>("", iw::FontMeshConfig { 15, iw::BOT_RIGHT });

	ttip->zIndex = 4;
	desc->zIndex = 4;
	stat->zIndex = 4;
	cost->zIndex = 4;

	m_text_money = m_screen->CreateElement<UI_Text>("", iw::FontMeshConfig{10});
	UpdateMoneyText(0);

	m_button_reform = m_screen->CreateElement<UI_Button>(itembg);
	m_button_reform->CreateElement<UI_Text>("Reform", iw::FontMeshConfig { 100 });
	m_button_reform->onClick = [this]()
	{
		Console->QueueCommand("game-start");

		UpgradeSet set;
		Bus->push<ApplyUpgradeSet_Event>(set);
	};

	m_background   ->zIndex =  0;
	m_tooltip      ->zIndex =  3;
	m_upgrades     ->zIndex = -1;
	m_bought       ->zIndex =  0;
	m_text_money   ->zIndex =  1;
	m_button_reform->zIndex =  1;

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

	m_text_money->width  = m_background->width;
	m_text_money->height = m_background->width;
	m_text_money->y = -300;
	m_text_money->x =  300;

	// could round table size to pixel grid;

	m_upgrades->height =  m_background->height * .64f;
	m_upgrades->width  =  m_background->width  * .85f;
	m_upgrades->y      = -m_background->height * .2f - 4;
	m_upgrades->x      = 8;

	m_upgrades->rowHeight = m_upgrades->height / 8.f;
	for (float& width : m_upgrades->colWidth)
	{
		width = m_upgrades->rowHeight;
	}

	float p = m_background->width / 200.f * 2;

	m_upgrades->rowPadding = p;
	m_upgrades->colPadding = { p, p, p, p, p };

	// bought

	m_bought->height =  m_background->height * .5f;
	m_bought->width  =  m_background->width  * .7f;
	m_bought->y      = 0;
	m_bought->x      = m_bought->width;

	m_bought->rowHeight = 50;
	m_bought->colWidth = { 50, 500 };

	m_bought->rowPadding = 5;
	m_bought->colPadding = { 5, 5 };

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
		//     |				   10,200 R  |
		//     o-----------------------------o
	}

	m_tooltip->active = false;

	for (Upgrade_Table::UI_Row& row : m_upgrades->rows)
	{
		for (size_t i = 0; i < m_upgrades->count; i++)
		{
			UpgradeDescription* upgrade = row.Data<UpgradeDescription*>(i);

			if (!upgrade)
			{
				continue;
			}

			UI_Button* button = row.Elem(i)->GetElement<UI_Button>();
			UI_Image* label = button->GetElement<UI_Image>();

			iw::Color* color = label->mesh.Material->Get<iw::Color>("color");

			button->clickActive = upgrade->Cost <= m_money;

			*color = button->clickActive 
				? iw::Color(1.f)
				: iw::Color(1.f, .5f, .5f, 1.f);

		}
	}

	float padding = 50;

	m_button_reform->x =  m_background->width  - 200 - padding;
	m_button_reform->y = -m_background->height + 50  + padding;
	m_button_reform->width  = 200;
	m_button_reform->height = 50;

	ButtonUpdate();
}
