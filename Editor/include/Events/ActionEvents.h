#pragma once

#include "iw/engine/Events/ActionEvents.h"

namespace iw {
	enum class Actions
		: int
	{
		JUMP, RIGHT, FORWARD, USE,
		SPAWN_CIRCLE_TEMP,
		RESET_LEVEL
	};

	struct JumpEvent
		: ToggleEvent
	{
		JumpEvent(
			bool active)
			: ToggleEvent(iw::val(Actions::JUMP), active)
		{}
	};

	struct RightEvent
		: ToggleEvent
	{
		RightEvent(
			bool active)
			: ToggleEvent(iw::val(Actions::RIGHT), active)
		{}
	};

	struct ForwardEvent
		: ToggleEvent
	{
		ForwardEvent(
			bool active)
			: ToggleEvent(iw::val(Actions::FORWARD), active)
		{}
	};

	struct UseEvent
		: SingleEvent
	{
		UseEvent()
			: SingleEvent(iw::val(Actions::USE))
		{}
	};

	struct ResetLevelEvent
		: SingleEvent
	{
		ResetLevelEvent()
			: SingleEvent(iw::val(Actions::RESET_LEVEL))
		{}
	};
}
