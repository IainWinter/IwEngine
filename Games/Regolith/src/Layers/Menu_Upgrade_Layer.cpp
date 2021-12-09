#include "Layers/Menu_Upgrade_Layer.h"

int Menu_Upgrade_Layer::Initialize()
{
	RegisterImage("x.png");
	RegisterImage("upgrade_healthefficiency.png");
	RegisterImage("upgrade_laserefficiency.png");
	RegisterImage("upgrade_thrusteracceleration.png");
	RegisterImage("upgrade_thrusterimpulse.png");
	RegisterImage("upgrade_thrustermaxspeed.png");

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

	//ImFontAtlas* fonts = ImGui::GetIO().Fonts;

	//////font = fonts->AddFontFromFileTTF("C:/dev/IwEngine/_assets/fonts/ttf/verdana.ttf", 15);
	////fonts->Build();

	return 0;
}

void Menu_Upgrade_Layer::UI()
{
	ImGui::PushFont(nullptr);

	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		ImGui::Text(iw::to_string(money).c_str());

		UpgradeTable();
		ActiveTable();
		Tooltip();
		Buttons();
	}
	ImGui::End();
	
	ImGui::PopStyleVar(1);

	ImGui::PopFont();
}

void Menu_Upgrade_Layer::UpgradeTable()
{
	float w = bg_w * .8f;
	float h = bg_h * .5f;

	float x = bg_x     + padding_1;
	float y = bg_h - h - padding_1;

	float upgradeSize = w / 8;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Upgrades", ImVec2(w, h), true, commonFlags);
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
				
				int frame   = 0;
				ImVec2 size = ImVec2(upgradeSize, upgradeSize);
				ImVec2 uv0  = ImVec2(0, 0);
				ImVec2 uv1  = ImVec2(1, 1);
				ImVec4 bg   = ImVec4(0, 0, 0, 0);
				ImVec4 tint = ImVec4(1, 1, 1, 1);

				if (!upgrade)
				{
					ImGui::Dummy(size);
					continue;
				}

				bool afford = upgrade->Cost <= money;

				if (!afford)
				{
					tint = ImVec4(1, .5, .5, 1);
				}

				if (   ImGui::ImageButton(imgs[upgrade->TexturePath], size, uv0, uv1, frame, bg, tint)
					&& afford) // always draw button
				{
					money -= upgrade->Cost;
					
					active.emplace_back(i, upgrade);
					upgrade = nullptr;
				}

				if (ImGui::IsItemHovered())
				{
					tooltip = upgrade;
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}

void Menu_Upgrade_Layer::ActiveTable()
{
	float w = bg_w * .8f;
	float h = 200;
	float x = bg_x + (bg_w - w) * .5f;
	float y = 0;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Active", ImVec2(w, h), true, commonFlags);
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
	ImGui::EndChild();
}

void Menu_Upgrade_Layer::Tooltip()
{
	if (!tooltip) return;

	float w = bg_w * .45f;
	float h = w * 9 / 16.f;
	float x = ImGui::GetMousePos().x + padding_01;
	float y = ImGui::GetMousePos().y + padding_01;

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGuiWindowFlags flags = commonFlags;
	flags &= ~(ImGuiWindowFlags_NoBringToFrontOnFocus);
	flags |= ImGuiWindowFlags_NoInputs;

	ImGui::Begin("Tooltip", nullptr, flags);

	ImGui::Text(tooltip->Name.c_str());
	ImGui::Text(tooltip->Description.c_str());
	ImGui::Text(tooltip->Stat.c_str());
	ImGui::Text("%d R", tooltip->Cost);

	ImGui::End();

	tooltip = nullptr; // reset for next frame
}

void Menu_Upgrade_Layer::Buttons()
{
	float w = bg_w * .25      - padding_01 * 2;
	float h = bg_w * .1f      - padding_01 * 2;
	float x = bg_x + bg_w - w - padding_01;
	float y = bg_h        - h - padding_01;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Buttons", ImVec2(w, h), true, commonFlags);
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.x -= ImGui::GetStyle().WindowPadding.x * 2;
		size.y -= ImGui::GetStyle().WindowPadding.y * 2;

		if (ImGui::Button("Reform", size)) // set the size of this button
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
	ImGui::EndChild();
}
