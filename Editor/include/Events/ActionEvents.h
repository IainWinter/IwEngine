#pragma once

#include "iw/engine/Events/ActionEvent.h"

namespace IW {
	enum class Actions
		: int
	{
		JUMP, USE
	};

	struct JumpEvent
		: ToggleActionEvent
	{
		JumpEvent(
			bool active)
			: ToggleActionEvent(iw::val(Actions::JUMP), active)
		{}
	};
}
