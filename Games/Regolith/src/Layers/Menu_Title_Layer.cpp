#include "Layers/Menu_Title_Layer.h"

#include "gl/glew.h"
#include "iw/renderer/Platform/OpenGL/GLErrorCatch.h"

int Menu_Title_Layer::Initialize()
{
	RegisterImage("ui_player_border.png");

	SubmitTempScoreAndGetId();

	font_regular = iwFont("verdana_36");
	font_title   = iwFont("verdana_92");

	//font_regular->DisplayOffset.y = font_regular->FontSize / 3.f;
	//font_title  ->DisplayOffset.y = font_title  ->FontSize / 3.f;

	//GL(glBindTexture  (GL_TEXTURE_2D, (GLuint)ImGui::GetIO().Fonts->TexID));
	//GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	////GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
	//GL(glBindTexture  (GL_TEXTURE_2D, 0));

	//font_regular->G((ImWchar)'1')->AdvanceX = font_regular->FindGlyph((ImWchar)'4')->AdvanceX;

	return 0;
}

void Menu_Title_Layer::UI()
{
	connection.StepResults();

	ImGui::GetIO().FontGlobalScale = bg_w / 800;

	ImGui::SetNextWindowPos (ImVec2(bg_x, bg_y));
	ImGui::SetNextWindowSize(ImVec2(bg_w, bg_h));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);

	ImGui::Begin("Main Menu", nullptr, commonFlags);
	{
		ImGui::PushFont(font_regular);
		{
			PlayerImage();
			ScoreTable();
			Buttons();
		}
	
		ImGui::PushFont(font_title);
		{
			Title();
		}

		ImGui::PopFont();
		ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleVar(1);
}

void Menu_Title_Layer::PlayerImage()
{
	float w = bg_w * .33f;
	float h = w;

	float x = bg_x + (bg_w - w) * .5;
	float y = bg_y + (bg_h - h) * .1;
	
	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("PlayerImage", ImVec2(w, h), true, commonFlags);
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.x -= ImGui::GetStyle().WindowPadding.x * 2;
		size.y -= ImGui::GetStyle().WindowPadding.y * 2;

		ImGui::Image(imgs["ui_player_border.png"], size);
	}
	ImGui::EndChild();
}

void Menu_Title_Layer::Title()
{
	const char* title = "SCORE";

	float w = bg_w;
	float h = bg_h * .1;

	float x = bg_x;
	float y = bg_h * .5 - h;

	ImVec2 textSize = ImGui::CalcTextSize(title);
	
	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Scores", ImVec2(w, h), true, commonFlags);
	{
		ImGui::SetCursorPosX((w - textSize.x) * 0.5f);
		ImGui::SetCursorPosY((h - textSize.y) * 0.5f);
		ImGui::Text(title);
	}
	ImGui::EndChild();
}

void Menu_Title_Layer::ScoreTable()
{
	float w = bg_w * .8f;
	float h = bg_h * .4f;

	float x = bg_x     + padding_1;
	float y = bg_h - h - padding_1;

	float upgradeSize = w / 8;

	ImGui::SetNextWindowPos(ImVec2(x, y));
	ImGui::SetNextWindowFocus();

	ImGui::BeginChild("Upgrades", ImVec2(w, h), true, commonFlags);
	{
		ImGui::BeginTable("Upgrades", 3);
		{
			ImGui::TableSetupColumn("Order", ImGuiTableColumnFlags_WidthStretch, .25);
			ImGui::TableSetupColumn("Score", ImGuiTableColumnFlags_WidthStretch, .5);
			ImGui::TableSetupColumn("Name",  ImGuiTableColumnFlags_WidthStretch);

			for (int i = 0; i < scores.size(); i++)
			{
				HighscoreRecord& record = scores.at(i);
				
				ImGui::TableNextRow();
				
				if (gameId != record.GameId)
				{
					ImGui::Text("%d", record.Order);
					ImGui::TableNextCell();
					ImGui::Text("%d", record.Score);
					ImGui::TableNextCell();
					ImGui::Text(record.Name.c_str());
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
			}
		}
		ImGui::EndTable();
	}
	ImGui::EndChild();
}

void Menu_Title_Layer::Buttons()
{
	float w = bg_w * .45      - padding_01 * 2;
	float h = bg_w * .1f      - padding_01 * 2;
	float x = bg_x + bg_w - w - padding_01;
	float y = bg_h        - h - padding_01;

	ImGui::SetNextWindowPos(ImVec2(x, y));

	ImGui::BeginChild("Buttons", ImVec2(w, h), true, commonFlags);
	{
		ImVec2 size = ImGui::GetWindowSize();
		size.x -= ImGui::GetStyle().WindowPadding.x * 3;
		size.y -= ImGui::GetStyle().WindowPadding.y * 2;
		size.x /= 2;

		if (ImGui::Button("Quit", size)) // set the size of this button
		{
			SubmitScoreAndExit("quit");
		}

		ImGui::SameLine();

		if (ImGui::Button("Upgrade", size)) // set the size of this button
		{
			SubmitScoreAndExit("game-upgrade");
		}
	}
	ImGui::EndChild();
}

void Menu_Title_Layer::SubmitTempScoreAndGetId()
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

			//m_table_highScore->scrollOffset 
			//	= m_table_highScore->Row(m_playerRowId)->index
			//	* (m_table_highScore->rowHeight * 2.f + m_table_highScore->rowPadding) 
			//	- m_table_highScore->height 
			//	+ m_table_highScore->rowHeight;
		};

		connection.AsyncRequest(request);
	};

	connection.AsyncRequest(request);
}

void Menu_Title_Layer::SubmitScoreAndExit(
	const std::string& command)
{
	Console->QueueCommand(command);

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
