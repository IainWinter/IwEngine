#include "Layers/Menu_Title_Layer.h"

int Menu_Title_Layer::Initialize()
{
	RegisterImage("x.png");
	RegisterImage("upgrade_healthefficiency.png");
	RegisterImage("upgrade_laserefficiency.png");
	RegisterImage("upgrade_thrusteracceleration.png");
	RegisterImage("upgrade_thrusterimpulse.png");
	RegisterImage("upgrade_thrustermaxspeed.png");

	commonFlags = 
		  ImGuiWindowFlags_NoDecoration 
		| ImGuiWindowFlags_NoResize 
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoBringToFrontOnFocus;

	upgrades = {
		MakeThrusterImpulse(),      nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		MakeThrusterAcceleration(), nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		MakeThrusterMaxSpeed(),     MakeHealthPickupEfficiency(), MakeLaserPickupEfficiency(), nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
	};

	return 0;
}

void Menu_Title_Layer::ImGui()
{
	float window_w = ImGui::GetIO().DisplaySize.x;
	float window_h = ImGui::GetIO().DisplaySize.y;

	window_w = ImGui::GetIO().DisplaySize.x;
	window_h = ImGui::GetIO().DisplaySize.y;

	bg_h = window_h;
	bg_w = bg_h * .8f;
	bg_x = (window_w - bg_w) * .5f;
	bg_y = 0.f;

	ActiveTable();
	UpgradeTable();
	Background();
}

void Menu_Title_Layer::Background()
{
	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		ImGui::Image(imgs["x.png"], ImVec2(100, 100));
	}
	ImGui::End();
	
	ImGui::PopStyleVar(1);
}

void Menu_Title_Layer::UpgradeTable()
{
	//ImGui::SetNextViewport

	float w = bg_w * .8f;
	float h = w;
	float x = bg_x + (bg_w - w) * .5f;
	float y = bg_h - h * 1.125f;

	float upgradeSize = w / 8;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Upgrades", nullptr, commonFlags);
	{
		ImGui::BeginTable("Upgrades", 8);
		{
			ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
			ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
			ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthAlwaysAutoResize);

			for (int i = 0; i < upgrades.size(); i++)
			{
				UpgradeDescription*& upgrade = upgrades.at(i);

				ImGui::TableNextCell();
				
				if (!upgrade)
				{
					continue;
				}

				if (ImGui::ImageButton(imgs[upgrade->TexturePath], ImVec2(upgradeSize, upgradeSize)))
				{
					active.emplace_back(i, upgrade);
					upgrade = nullptr;
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();

	ImGui::PopStyleVar(1);
}

void Menu_Title_Layer::ActiveTable()
{
	float w = bg_w * .8f;
	float h = 200;
	float x = bg_x + (bg_w - w) * .5f;
	float y = 0;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Active", nullptr, commonFlags);
	{
		ImGui::BeginTable("Upgrades", 2);
		{
			ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
			ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthAlwaysAutoResize);

			for (int i = 0; i < active.size(); i++)
			{
				auto [index, upgrade] = active.at(i);

				ImGui::TableNextCell();
				if (ImGui::ImageButton(imgs[upgrade->TexturePath], ImVec2(50, 50)))
				{
					upgrades[index] = upgrade;
					active.erase(active.begin() + i);
					i--;
				}

				ImGui::TableNextCell();
				ImGui::Text(upgrade->Name.c_str());

			}
		}
		ImGui::EndTable();
	}
	ImGui::End();

	ImGui::PopStyleVar(1);


}

#define LoadTexture(x) Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)
#define LoadFont(x) Asset->Load<iw::Font>(std::string("fonts/") + x)

void Menu_Title_Layer::RegisterImage(const std::string& str)
{
	auto img = LoadTexture(str);
	img->SetFilter(iw::NEAREST);
	img->Initialize(Renderer->Now->Device);

	imgs[str] = (void*)img->Handle()->Id();
}
