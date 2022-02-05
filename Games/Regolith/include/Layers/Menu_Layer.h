#pragma once

#include "iw/engine/Layers/ImGuiLayer.h"

#include "Events.h"
#include "Assets.h"

#include <string>
#include <unordered_map>

#define LoadTexture(x) Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)
#define LoadFont(x) Asset->Load<iw::Font>(std::string("fonts/") + x)

struct Menu_Layer : iw::Layer
{
	float window_w;
	float window_h;

	float bg_h;
	float bg_w;
	float bg_x;
	float bg_y;

	float padding_1;
	float padding_01;

	ImGuiWindowFlags commonFlags;
	ImGuiWindowFlags commonFlagsFocus;

	Menu_Layer(
		const std::string& name
	)
		: iw::Layer  (name)
		, window_w   (0.f) 
		, window_h   (0.f)
		, bg_h       (0.f)
		, bg_w       (0.f)
		, bg_x       (0.f)
		, bg_y       (0.f)
		, padding_1  (0.f)
		, padding_01 (0.f)
	{
		commonFlags = 
			  ImGuiWindowFlags_NoDecoration 
			| ImGuiWindowFlags_NoResize 
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus;

		commonFlagsFocus =
			  ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove;
	}

	void ImGui()
	{
		window_w = ImGui::GetIO().DisplaySize.x;
		window_h = ImGui::GetIO().DisplaySize.y;

		bg_h = window_h;
		bg_w = bg_h * .8f;
		bg_x = (window_w - bg_w) * .5f;
		bg_y = 0.f;

		bg_h = floor(bg_h);
		bg_w = floor(bg_w);
		bg_x = floor(bg_x);
		bg_y = floor(bg_y);

		padding_1  = bg_w * .1f;
		padding_01 = bg_w * .01f;

		ImGui::GetIO().FontGlobalScale = bg_w / 800;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0, 0));

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::PushStyleColor(ImGuiCol_Border,   ImVec4(0, 0, 0, 0));

		UI();

		ImGui::PopStyleVar  (3);
		ImGui::PopStyleColor(2);

		Layer::ImGui();
	}

	virtual void UI() = 0;
};
