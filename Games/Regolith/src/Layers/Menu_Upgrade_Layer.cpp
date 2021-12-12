#include "Layers/Menu_Upgrade_Layer.h"

int Menu_Upgrade_Layer::Initialize()
{
	RegisterImage("x.png");

	upgrades = {
		MakeThrusterBreaking(),     nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		MakeThrusterAcceleration(), nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		MakeThrusterMaxSpeed(),     MakeHealthPickupEfficiency(), MakeLaserPickupEfficiency(), nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr,                    nullptr,                      nullptr,                     nullptr, nullptr, nullptr, nullptr, nullptr,
	};

	for (UpgradeDescription* description : upgrades)
	{
		if (!description) continue;
		RegisterImage(description->TexturePath);
	}

	font_regular   = iwFont("verdana_36");
	font_paragraph = iwFont("verdana_18");

	UpdateSpeedGraph();

	return 0;
}

void Menu_Upgrade_Layer::UI()
{
	ImGui::GetIO().FontGlobalScale = bg_w / 800;

	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		ImGui::PushFont(font_regular);

		ImGui::Text(iw::to_string(money).c_str());
		UpgradeTable();
		ActiveTable();
		Tooltip();
		Buttons();
		
		//UpgradePreview();

		ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleVar(1);
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

			ImGui::SetScrollHereY();

			for (int i = 0; i < upgrades.size(); i++)
			{
				UpgradeDescription*& upgrade = upgrades.at(i);

				ImGui::TableNextCell();
				
				ImVec2 size = ImVec2(upgradeSize, upgradeSize);
				ImVec2 uv0  = ImVec2(0, 0);
				ImVec2 uv1  = ImVec2(1, 1);
				ImVec4 bg   = ImVec4(0, 0, 0, 0);
				
				if (!upgrade)
				{
					ImGui::Dummy(size);
					continue;
				}

				bool bought = upgrade->NumberBought >= upgrade->CostStatTable.size();
				bool afford = !bought && upgrade->CostStatTable[upgrade->NumberBought].first <= money;

				int frame   = 0;
				ImVec4 tint = bought ? ImVec4(.5, .5, .5, 1)
								     : afford 
									   ? ImVec4(1, 1, 1, 1)
									   : ImVec4(1, .5, .5, 1);

				if (   ImGui::ImageButton(imgs[upgrade->TexturePath], size, uv0, uv1, frame, bg, tint)
					&& afford)
				{
					money -= upgrade->CostStatTable[upgrade->NumberBought].first;
					upgrade->NumberBought += 1;
				}

				if (ImGui::IsItemHovered())
				{
					if (   ImGui::IsMouseClicked(ImGuiMouseButton_Right)
						&& upgrade->NumberBought > 0)
					{
						upgrade->NumberBought -= 1;
						money += upgrade->CostStatTable[upgrade->NumberBought].first;
					}

					tooltip = upgrade;
				}

				for (int j = 0; j < upgrade->NumberBought; j++)
				{
					float r = upgradeSize / 20.f;

					ImVec2 pos = ImGui::GetCursorPos();
					pos.x += x;
					pos.y += y;
					pos.x += upgradeSize - r * 2.f;

					pos.x -= 3.f * r * (j / 6);
					pos.y -= 3.f * r * (j % 6 + 1);

					ImU32 color = iw::Color(0.98, .7, .01).to32();

					ImGui::GetWindowDrawList()->AddCircleFilled(pos, r, color, 36);
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}

void Menu_Upgrade_Layer::ActiveTable()
{
	float w = bg_w * .5;
	float h = bg_h * .5f - padding_1 * 2;
	float x = bg_x + (bg_w - w) * .5;
	float y = padding_01;

	//int number = 8;

	float upgradeSize = w / 8;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Active", ImVec2(w, h), true, commonFlags);
	{
		ImGui::BeginTable("Upgrades", 2);
		{
			for (int i = 0; i < 2; i++)
			{
				ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
			}

			for (int i = 0; i < active.size(); i++)
			{
				auto [index, upgrade] = active.at(i);

				ImGui::TableNextCell();
				if (ImGui::ImageButton(imgs[upgrade->TexturePath], ImVec2(upgradeSize, upgradeSize)))
				{
					upgrades[index] = upgrade;
					active.erase(active.begin() + i);
					i--;
				}

				ImGui::TableNextCell();

				const char* text = upgrade->Name.c_str();
				//ImVec2 textSize = ImGui::CalcTextSize(text);
				//ImGui::SetCursorPosY(ImGui::GetCursorPos().y + (h - textSize.y) * 0.5f);
				ImGui::PushFont(font_paragraph);
				
				ImGui::Text(text);

				ImGui::PopFont();
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

	ImGui::PushFont(font_paragraph);

	ImGui::Text(tooltip->Name.c_str());
	ImGui::Text(tooltip->Description.c_str());

	if (tooltip->NumberBought >= tooltip->CostStatTable.size())
	{
		ImGui::Text("Bought out!");
	}

	else 
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
		ImGui::Text("%d", tooltip->CostStatTable[tooltip->NumberBought].second);
		ImGui::PopStyleColor(1);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.98, .7, .01, 1));
		ImGui::Text("%d R", tooltip->CostStatTable[tooltip->NumberBought].first);
		ImGui::PopStyleColor(1);
	}

	ImGui::PopFont();

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
			UpgradeSet set;
			for (UpgradeDescription* description : upgrades)
			{
				if (!description || description->NumberBought == 0) continue;
				set.AddUpgrade(description->Upgrade);
			}

			Bus->push<ApplyUpgradeSet_Event>(set);
			Console->QueueCommand("game-start");
		}
	}
	ImGui::EndChild();
}

void Menu_Upgrade_Layer::UpgradePreview()
{
	float w = bg_w * .5;
	float h = bg_h * .5f - padding_1 - padding_01 * 2;
	float x = bg_x + bg_w * .5;
	float y = padding_01;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Graph", ImVec2(w, h), true, commonFlags);
	ImGui::PlotLines("", vals.data(), vals.size(), 0, nullptr, 0, 250, ImVec2(300, 300), 4);
	ImGui::EndChild();
}

void Menu_Upgrade_Layer::UpdateSpeedGraph()
{
	player = {};
	float dt = iw::FixedTime();
	float v = 0;

	while (v < player.u_speed)
	{
		vals.push_back(v);
		v = player.CalcVel(v, 1, 1, dt);
	}

	for (int i = vals.size(); i >= 0; i--) // make max speed be a third of the plot
	{
		vals.push_back(v);
	}

	while (v > 0)
	{
		vals.push_back(v);
		v = player.CalcVel(v, 1, 0, dt);
	}

	vals.push_back(0);
}
