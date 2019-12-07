#pragma once

#include "iw/engine/Events/ActionEvents.h"

namespace IW {
	enum class Actions
		: int
	{
		JUMP, RIGHT, FORWARD, USE
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
}
