#include "MenuParts.h"

#include "gl/glew.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"

void Highscores_MenuParts::ScoreTable(
	float x,
	float y,
	float w,
	float h)
{
	connection.StepResults();

	int height = ImGui::CalcTextSize("O").y + 5;

	ImGui::SetNextWindowPos(ImVec2(x,  y - height));
	ImGui::BeginChild("Highscores title", ImVec2(w, ImGui::CalcTextSize("A").y));
	{
		if (ImGui::BeginTable("Upgrades", 3))
		{
			ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthStretch, .5);
			ImGui::TableSetupColumn("Score", ImGuiTableColumnFlags_WidthStretch, .5);
			ImGui::TableSetupColumn("Name",  ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableNextCell();
			ImGui::Text("Order");
			ImGui::TableNextCell();
			ImGui::Text("Score");
			ImGui::TableNextCell();
			ImGui::Text("Name");

			//ImGui::GetForegroundDrawList()->AddLine(
			//	ImVec2(
			//		ImGui::GetWindowPos().x,
			//		ImGui::GetWindowPos().y + ImGui::GetCursorPos().y - 5),
			//	ImVec2(
			//		ImGui::GetWindowPos().x + ImGui::GetWindowWidth() - 15,
			//		ImGui::GetWindowPos().y + ImGui::GetCursorPos().y - 5),
			//	iw::Color(1).to32()
			//);

			//ImGui::GetForegroundDrawList()->AddLine(
			//	ImVec2(
			//		ImGui::GetWindowPos().x - 5,
			//		ImGui::GetWindowPos().y),
			//	ImVec2(
			//		ImGui::GetWindowPos().x - 5,
			//		ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
			//	iw::Color(1).to32()
			//);

			ImGui::EndTable();
		}
	}
	ImGui::EndChild();

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::BeginChild("Highscores", ImVec2(w, h));
	{
		if (ImGui::BeginTable("Upgrades", 3))
		{
			ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthStretch, .5);
			ImGui::TableSetupColumn("Score", ImGuiTableColumnFlags_WidthStretch, .5);
			ImGui::TableSetupColumn("Name",  ImGuiTableColumnFlags_WidthStretch);

			for (int i = 0; i < scores.size(); i++)
			{
				HighscoreRecord& record = scores.at(i);
				
				ImGui::PushStyleColor(ImGuiCol_TableRowBg, ImVec4(.1, .1, .1, 1));
				ImGui::TableNextRow();
				
				if (gameId != record.GameId)
				{
					ImGui::Text("%d", record.Order);
					ImGui::TableNextCell();
					ImGui::Text("%d", record.Score);
					ImGui::TableNextCell();

					RightAlign(record.Name);
					ImGui::Text(record.Name.c_str());

					if (i == scores.size() - 1 && ImGui::IsItemVisible())
					{
						LoadMoreScoresBelow();
					}
				}

				else 
				{
					ImVec2 min, max;
					float r = sin(iw::TotalTime() * 10)              * .5 + 1;
					float g = sin(iw::TotalTime() * 11 + 2/3*iw::Pi) * .5 + 1;
					float b = sin(iw::TotalTime() * 12 + 4/3*iw::Pi) * .5 + 1;

					ImGui::PushStyleColor(ImGuiCol_Text,    ImVec4(r, g, b, 1));
					ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
					ImGui::PushStyleVar  (ImGuiStyleVar_FramePadding, ImVec2(0, 0));

					ImGui::Text("%d", record.Order);
					ImGui::TableNextCell();
					ImGui::Text("%d", record.Score);
					ImGui::TableNextCell();
					ImGui::InputText("Input Name", record.Name.data(), record.Name.capacity(), ImGuiInputTextFlags_None);

					ImGui::PopStyleColor(2);
					ImGui::PopStyleVar  (1);

					if (scroll_table)
					{
						scroll_table = false;
						ImGui::SetScrollHereY();
					}
				}

				ImGui::PopStyleColor();
			}
			ImGui::EndTable();
		}
	}
	ImGui::EndChild();
}

void Highscores_MenuParts::SubmitTempNameAndGetGameId(
	int score)
{
	iw::HttpRequest<std::string, iw::JsonSerializer> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.niceyam.com";
	request.Resource = "/regolith/php/submit_highscore.php";
	request.SetArgument("name",  "Anon");
	request.SetArgument("score", score);
	
	request.OnResult = [this](
		std::string& uuid)
	{
		gameId = uuid;

		iw::HttpRequest<std::vector<HighscoreRecord>, iw::JsonSerializer> request;
		request.Ip       = "71.233.150.182";
		request.Host     = "data.niceyam.com";
		request.Resource = "/regolith/php/get_highscores_around.php";
		request.SetArgument("game_id", uuid);

		request.OnResult = [this](
			std::vector<HighscoreRecord>& records)
		{
			for (HighscoreRecord& record : records)
			{
				scores.emplace_back(record);

				if (record.GameId == gameId)
				{
					record.Name.reserve(33);
				}
			}
		};

		connection.AsyncRequest(request);
	};

	connection.AsyncRequest(request);
}

void Highscores_MenuParts::UpdateTempNameWithRealName()
{
	// this needs to update the database at game_id with the name

	//iw::HttpRequest<std::string, iw::NoSerializer> request;
	//request.Ip       = "71.233.150.182";
	//request.Host     = "data.niceyam.com";
	//request.Resource = "/regolith/php/submit_highscore.php";

	//request.OnResult = [](std::string& str)
	//{
	//	LOG_INFO << str;
	//};

	//request.SetArgument("name", *name);
	//request.SetArgument("score", to_string(score));

	//connection.AsyncRequest(request);
}

void Highscores_MenuParts::LoadTopScore()
{
	if (loading) return;
	loading = true;

	iw::HttpRequest<HighscoreRecord, iw::JsonSerializer> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.niceyam.com";
	request.Resource = "/regolith/php/get_highscore_top.php";

	request.OnResult = [this](
		HighscoreRecord& record)
	{
		scores.emplace(scores.begin(), record);
		loading = false;
	};

	connection.AsyncRequest(request);
}

void Highscores_MenuParts::LoadMoreScoresAbove()
{
	if (loading) return;
	loading = true;

	std::string gameId = scores.front().GameId;

	iw::HttpRequest<std::vector<HighscoreRecord>, iw::JsonSerializer> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.niceyam.com";
	request.Resource = "/regolith/php/get_highscore_above.php";
	request.SetArgument("game_id", gameId);

	request.OnResult = [this](
		std::vector<HighscoreRecord>& records)
	{
		for (HighscoreRecord& record : records)
		{
			scores.emplace(scores.begin(), record);
		}
		loading = false;
	};

	connection.AsyncRequest(request);
}

void Highscores_MenuParts::LoadMoreScoresBelow()
{
	if (loading) return;
	loading = true;

	std::string gameId = scores.back().GameId;

	iw::HttpRequest<std::vector<HighscoreRecord>, iw::JsonSerializer> request;
	request.Ip       = "71.233.150.182";
	request.Host     = "data.niceyam.com";
	request.Resource = "/regolith/php/get_highscores_page.php";
	request.SetArgument("game_id", gameId);
	request.SetArgument("direction", "d");

	request.OnResult = [this](
		std::vector<HighscoreRecord>& records)
	{
		for (HighscoreRecord& record : records)
		{
			scores.emplace_back(record);
		}
		loading = false;
	};

	connection.AsyncRequest(request);
}

/*

	Update Table

*/

void Upgrade_MenuParts::UpgradeTable(float x, float y, float w, float h)
{
	float upgradePadding = 5;
	float upgradeSize = (w - upgradePadding * 5) / 7;

	ImGui::SetCursorPos(ImVec2(x, y));
	ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(3, 3));

	ImGui::BeginTable("Upgrades", 7, 0, ImVec2(w, h));
	{
		//ImGui::TableSetupColumn("0", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		//ImGui::TableSetupColumn("1", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		//ImGui::TableSetupColumn("2", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		
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

			int frame = 0;
			ImVec4 tint = bought ? ImVec4(.5, .5, .5, 1)
								 : afford 
									? ImVec4(1, 1, 1, 1)
									: ImVec4(1, .5, .5, 1);

			if (   ImageButton(Image(upgrade->TexturePath), size, uv0, uv1, frame, bg, tint)
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
				pos.x += bg_x;
				//pos.x += x + ImGui::GetWindowPos().x;
				//pos.y += y + ImGui::GetWindowPos().y;
				pos.x += upgradeSize - r * 2.f;

				pos.x -= 3.f * r * (j / 6);
				pos.y -= 3.f * r * (j % 6 + 1);

				ImU32 color = iw::Color(0.98, .7, .01).to32();

				ImGui::GetForegroundDrawList()->AddCircleFilled(pos, r, color, 36);
			}
		}
	}
	ImGui::EndTable();

	ImGui::PopStyleVar();
}

void Upgrade_MenuParts::ActiveTable(float x, float y, float w, float h)
{
	//int number = 8;

	float upgradeSize = w / 8;

	ImGui::SetCursorPos(ImVec2(x, y));

	ImGui::BeginTable("Upgrades", 2, 0, ImVec2(w, h));
	{
		for (int i = 0; i < 2; i++)
		{
			ImGui::TableSetupColumn("##", ImGuiTableColumnFlags_WidthAlwaysAutoResize);
		}

		for (int i = 0; i < active.size(); i++)
		{
			auto [index, upgrade] = active.at(i);

			ImGui::TableNextCell();
			if (ImGui::ImageButton(Image(upgrade->TexturePath), ImVec2(upgradeSize, upgradeSize)))
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

void Upgrade_MenuParts::Tooltip(float x, float y)
{
	if (!tooltip) return;

	ImGui::PushFont(font_regular);
	ImGui::SetCursorPosY(y);
	ImGui::SetCursorPosX(x);
	ImGui::Text(tooltip->Name.c_str());
	ImGui::PopFont();

	ImGui::PushFont(font_paragraph);
	
	ImGui::SetCursorPosX(x);
	ImGui::Text(tooltip->Description.c_str());

	if (tooltip->NumberBought >= tooltip->CostStatTable.size())
	{
		ImGui::SetCursorPosX(x);
		ImGui::Text("Bought out!");
	}

	else 
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
		ImGui::SetCursorPosX(x); 
		ImGui::Text("%d", tooltip->CostStatTable[tooltip->NumberBought].second);
		ImGui::PopStyleColor(1);

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.98, .7, .01, 1));
		ImGui::SetCursorPosX(x);
		ImGui::Text("%d R", tooltip->CostStatTable[tooltip->NumberBought].first);
		ImGui::PopStyleColor(1);
	}

	ImGui::PopFont();

	tooltip = nullptr; // reset for next frame
}
