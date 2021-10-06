#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/Assets.h"
#include "iw/engine/UI.h"

struct Menu_Layer : iw::Layer
{
	UI_Screen* m_screen;

	iw::HandlerFunc* m_handle;
	bool m_execute;
	bool m_last_execute;

	Menu_Layer(
		const std::string& name
	)
		: iw::Layer      (name)
					  
		, m_screen       (nullptr)

		, m_handle       (nullptr)
		, m_execute      (false)
		, m_last_execute (false)
	{}

	virtual void OnPush() override
	{
		if (m_screen)
		{
			Space->FindEntity(m_screen).Revive();
		}

		else {
			m_screen = Space->CreateEntity<UI_Screen>().Set<UI_Screen>();
		}

		m_handle = Console->AddHandler([&](
			const iw::Command& command) 
		{
			if (command.Verb == "execute")
			{
				m_execute = command.Active;
			}

			return false;
		});

		Layer::OnPush();
	}

	virtual void Destroy() override
	{
		Space->FindEntity(m_screen).Revive();
		Space->FindEntity(m_screen).Destroy();
		m_screen = nullptr;
	}

	virtual void OnPop() override
	{
		Space->FindEntity(m_screen).Kill();
		Console->RemoveHandler(m_handle);

		m_handle = nullptr;
		m_execute = false;
		m_last_execute = false;
	}
};
