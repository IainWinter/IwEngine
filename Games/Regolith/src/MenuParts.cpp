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

	ImGui::SetNextWindowPos (ImVec2(x,  y - height));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGui::Begin("Highscores title", nullptr, commonFlagsFocus);
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
	ImGui::End();

	ImGui::SetNextWindowPos (ImVec2(x, y));
	ImGui::SetNextWindowSize(ImVec2(w, h));

	ImGui::Begin("Highscores", nullptr, commonFlagsFocus);
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

					ImGui::SetCursorPosX(
						  ImGui::GetCursorPosX()
						+ ImGui::GetColumnWidth()
						- ImGui::CalcTextSize(record.Name.c_str()).x
						- ImGui::GetScrollX()
						- ImGui::GetStyle().ItemSpacing.x * 2);

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
	ImGui::End();
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
