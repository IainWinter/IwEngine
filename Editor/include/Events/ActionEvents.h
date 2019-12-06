#pragma once

#include "iw/engine/Events/ActionEvents.h"

namespace IW {
	enum class Actions
		: int
	{
		JUMP, RIGHT, FORWARD, USE
	};

	struct JumpEvent
		: ToggleActionEvent
	{
		JumpEvent(
			bool active)
			: ToggleActionEvent(iw::val(Actions::JUMP), active)
		{}
	};

	struct RightEvent
		: ToggleActionEvent
	{
		RightEvent(
			bool active)
			: ToggleActionEvent(iw::val(Actions::RIGHT), active)
		{}
	};

	struct ForwardEvent
		: ToggleActionEvent
	{
		ForwardEvent(
			bool active)
			: ToggleActionEvent(iw::val(Actions::FORWARD), active)
		{}
	};

	struct UseEvent
		: SingleActionEvent
	{
		UseEvent()
			: SingleActionEvent(iw::val(Actions::USE))
		{}
	};
}
