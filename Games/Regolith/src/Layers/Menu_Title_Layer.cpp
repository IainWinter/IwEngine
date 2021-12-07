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

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	Tooltip();
	ActiveTable();
	UpgradeTable();
	Buttons();
	Background();

	ImGui::PopStyleVar(1);
}

void Menu_Title_Layer::Background()
{
	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		//ImGui::Image(imgs["x.png"], ImVec2(100, 100));
	}
	ImGui::End();
	
	ImGui::PopStyleVar(1);
}

void Menu_Title_Layer::UpgradeTable()
{
	float w = bg_w * .8f;
	float h = bg_h * .5f;

	float p = (bg_w - w) * .5f;

	float x = bg_x + p;
	float y = bg_h - h - p;

	float upgradeSize = w / 8;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGui::Begin("Upgrades", nullptr, commonFlags);
	{
		ImGui::BeginTable("Upgrades", 8);
		{
			ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthFixed, upgradeSize);
			ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthFixed, upgradeSize);
			ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthFixed, upgradeSize);

			for (int i = 0; i < upgrades.size(); i++)
			{
				UpgradeDescription*& upgrade = upgrades.at(i);

				ImGui::TableNextCell();
				
				if (!upgrade)
				{
					continue;
				}

				void* img   = imgs[upgrade->TexturePath];
				int frame   = 0;
				ImVec2 size = ImVec2(upgradeSize, upgradeSize);
				ImVec2 uv0  = ImVec2(0, 0);
				ImVec2 uv1  = ImVec2(1, 1);
				ImVec4 bg   = ImVec4(0, 0, 0, 0);
				ImVec4 tint = ImVec4(1, 1, 1, 1);

				bool afford = upgrade->Cost <= money;

				if (!afford)
				{
					tint = ImVec4(1, .5, .5, 1);
				}

				if (   ImGui::ImageButton(img, size, uv0, uv1, frame, bg, tint)
					&& afford) // always draw button
				{
					money -= upgrade->Cost;
					
					active.emplace_back(i, upgrade);
					upgrade = nullptr; // maybe this should just disable?
				}

				if (ImGui::IsItemHovered())
				{
					tooltip = upgrade;
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();
}

void Menu_Title_Layer::ActiveTable()
{
	float w = bg_w * .8f;
	float h = 200;
	float x = bg_x + (bg_w - w) * .5f;
	float y = 0;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

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
					money += upgrade->Cost;
					
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
}

void Menu_Title_Layer::Tooltip()
{
	if (!tooltip) return;

	float w = bg_w * .45f;
	float h = w * 9 / 16.f;
	float x = ImGui::GetMousePos().x;
	float y = ImGui::GetMousePos().y;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGui::Begin("Tooltip", nullptr, commonFlags);

	ImGui::Text(tooltip->Name.c_str());
	ImGui::Text(tooltip->Description.c_str());
	ImGui::Text(tooltip->Stat.c_str());
	ImGui::Text(std::string(tooltip->Cost + "R").c_str());

	ImGui::End();

	tooltip = nullptr; // reset for next frame
}

void Menu_Title_Layer::Buttons()
{
	float p = bg_w * .01f; // get the ImGuiStyleVar_FramePadding
	
	float w = bg_w * .25 - p * 2;
	float h = bg_w * .1f - p * 2;
	float x = bg_x + bg_w - w - p;
	float y = bg_h - h - p;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGui::Begin("Buttons", nullptr, commonFlags);
	{
		if (ImGui::Button("Reform")) // set the size of this button
		{
			LOG_INFO << "Pressed reform button";

			UpgradeSet set;
			for (auto& [_, upgrade] : active)
			{
				set.AddUpgrade(upgrade->Create());
			}

			Bus->push<ApplyUpgradeSet_Event>(set);
			Console->QueueCommand("game-start");
		}
	}
	ImGui::End();
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
