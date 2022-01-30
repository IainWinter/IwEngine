#pragma once

#include "iw/engine/Layers/ImGuiLayer.h"
#include <unordered_map>

#define LoadTexture(x) Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)

struct Menu_Layer2 : iw::Layer
{
	std::unordered_map<std::string, void*> imgs;
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

	Menu_Layer2(
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

	void RegisterImage(const std::string& str)
	{
		auto img = LoadTexture(str);
		img->SetFilter(iw::NEAREST);
		img->Initialize(Renderer->Now->Device);

		imgs[str] = (void*)(size_t)img->Handle()->Id();
	}
};

#undef LoadTexture

#include "iw/engine/Layer.h"
#include "iw/engine/UI.h"
#include "Assets.h"

#define LoadTexture(x) Asset->Load<iw::Texture>(std::string("textures/SpaceGame/") + x)
#define LoadFont(x) Asset->Load<iw::Font>(std::string("fonts/") + x)

struct UI_Layer : iw::Layer
{
	UI_Screen* m_screen;

	UI_Layer(
		const std::string& name
	)
		: iw::Layer (name)
		, m_screen  (nullptr)
	{}

	virtual void Destroy() override
	{
		Space->FindEntity(m_screen).Destroy();
		m_screen = nullptr;

		Layer::Destroy();
	}

	virtual void OnPush() override
	{
		if (m_screen)
		{
			Space->FindEntity(m_screen).Revive();
		}

		else {
			m_screen = Space->CreateEntity<UI_Screen>().Set<UI_Screen>();
			m_screen->defaultMesh = iw::ScreenQuad().MakeInstance();
			m_screen->defaultMesh.Material = A_material_texture_cam->MakeInstance();

			m_screen->defaultFont = LoadFont("basic.fnt");
			m_screen->defaultFont->m_material = A_material_font_cam->MakeInstance();
			m_screen->defaultFont->m_material->SetTexture("texture", m_screen->defaultFont->GetTexture(0));
			m_screen->defaultFont->m_material->Set       ("isFont",     1.0f);
			m_screen->defaultFont->m_material->Set       ("font_edge",  0.5f);
			m_screen->defaultFont->m_material->Set       ("font_width", 0.25f);
			m_screen->defaultFont->GetTexture(0)->SetFilter(iw::NEAREST);
		}

		Layer::OnPush();
	}

	virtual void OnPop() override
	{
		Space->FindEntity(m_screen).Kill();

		Layer::OnPop();
	}
};

struct Menu_Layer : UI_Layer
{
	iw::HandlerFunc* m_handle;
	bool m_execute;
	bool m_last_execute;

	Menu_Layer(
		const std::string& name
	)
		: UI_Layer       (name)
					  
		, m_handle       (nullptr)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	void ButtonUpdate()
	{
		// find all buttons

		std::vector<UI_Base*> clickables;

		m_screen->WalkTree([&](UI_Base* ui, UI_Base* parent)
		{
			if (dynamic_cast<UI_Clickable*>(ui))
			{
				clickables.push_back(ui);
			}
		});

		for (UI_Base* ui : clickables)
		{
			UI_Clickable* clickable = dynamic_cast<UI_Clickable*>(ui);

			if (    ui->active
				&& clickable->clickActive
				&& ui->IsPointOver(m_screen->LocalMouse()))
			{
				if (clickable->whileMouseHover)
				{
					clickable->whileMouseHover();
				}

				if (    m_execute
					&& clickable->whileMouseDown)
				{
					clickable->whileMouseDown();
				}

				else
				if (    m_last_execute
					&& clickable->onClick)
				{
					clickable->onClick();
				}
			}
			// top reset buttons but this isnt needed if they are positioned
			//else
			//{
			//	ui->x = 0;
			//	ui->y = 0;
			//}
		}

		m_last_execute = m_execute;
	}
	
	virtual void OnPush() override
	{
		m_handle = Console->AddHandler([&](
			const iw::Command& command) 
		{
			if (command.Verb == "execute")
			{
				m_execute = command.Active;
			}

			return false;
		});

		UI_Layer::OnPush();
	}

	virtual void OnPop() override
	{
		Console->RemoveHandler(m_handle);

		m_handle = nullptr;
		m_execute = false;
		m_last_execute = false;

		UI_Layer::OnPop();
	}
};
