#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/UI.h"
#include "Assets.h"

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
		: UI_Layer      (name)
					  
		, m_handle       (nullptr)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	void ButtonUpdate()
	{
		// find all buttons

		std::vector<UI_Button*> buttons;

		m_screen->WalkTree([&](UI_Base* ui)
		{
			UI_Button* button = dynamic_cast<UI_Button*>(ui);
			if (button) buttons.push_back(button);
		});

		for (UI_Button* button : buttons)
		{
			float buttonOffsetTarget = 0;
			if (button->IsPointOver(m_screen->LocalMouse()))
			{
				buttonOffsetTarget = 10;

				if (m_execute)
				{
					buttonOffsetTarget = 0;
				}

				else
				if (   m_last_execute
					&& button->onClick)
				{
					button->onClick();
				}
			}
			button->offset = iw::lerp(button->offset, buttonOffsetTarget, iw::DeltaTime() * 20);

			// should have enum for effect

			button->y += floor(button->offset);
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
