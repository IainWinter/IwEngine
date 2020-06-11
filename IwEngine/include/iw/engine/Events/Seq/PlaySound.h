#pragma once

#include "EventTask.h"
#include "iw/audio/AudioSpaceStudio.h"

namespace iw {
namespace Engine {
	struct PlaySound
		: EventTask
	{
	private:
		std::string name;

	public:
		PlaySound(
			std::string name)
			: name(name)
		{}

		bool update() override {
			if (name.length() == 0) return true;

			Audio->AsStudio()->CreateInstance(name);

			return true;
		}
	};
}

	using namespace Engine;
}
