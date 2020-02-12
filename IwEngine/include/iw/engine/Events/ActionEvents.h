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

	struct ActionEvent
		: iw::event
	{
		int Action;

		ActionEvent(
			ActionEventType type,
			int action)
			: iw::event(iw::val(EventGroup::ACTION), iw::val(type))
			, Action(action)
		{}
	};

	struct ToggleEvent
		: ActionEvent
	{
		bool Active;

		ToggleEvent(
			int action,
			bool active)
			: ActionEvent(ActionEventType::TOGGLE, action)
			, Active(active)
		{}
	};

	struct SingleEvent
		: ActionEvent
	{
		SingleEvent(
			int action)
			: ActionEvent(ActionEventType::SINGLE, action)
		{}
	};
}

	using namespace Engine;
}
