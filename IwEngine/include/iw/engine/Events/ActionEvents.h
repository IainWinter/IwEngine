#pragma once

#include "iw/events/event.h"
#include "iw/common/Events/EventGroups.h"

namespace iw {
namespace Engine {
	enum class ActionEventType
		: short
	{
		TOGGLE,
		SINGLE
	};

	struct REFLECT ActionEvent
		: event
	{
		int Action;

		ActionEvent(
			ActionEventType type,
			int action
		)
			: event(val(EventGroup::ACTION), val(type))
			, Action(action)
		{}
	};

	struct REFLECT ToggleEvent
		: ActionEvent
	{
		bool Active;

		ToggleEvent(
			int action,
			bool active
		)
			: ActionEvent(ActionEventType::TOGGLE, action)
			, Active(active)
		{}
	};

	struct REFLECT SingleEvent
		: ActionEvent
	{
		SingleEvent(
			int action
		)
			: ActionEvent(ActionEventType::SINGLE, action)
		{}
	};
}

	using namespace Engine;
}
