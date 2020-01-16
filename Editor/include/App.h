#pragma once 

#include "iw/engine/EntryPoint.h"
#include "Layers/ToolLayer.h"

namespace IW {
inline namespace Editor {
	class App
		: public IW::Application
	{
	public:
	private:
		ToolLayer* toolbox;
		bool active;

	public:
		App();

		int Initialize(
			IW::InitOptions& options) override;

		bool HandleCommand(
			const Command& command) override;
	};
}
}
