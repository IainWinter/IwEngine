#pragma once 

#include "iw/engine/EntryPoint.h"
#include "Layers/ToolLayer.h"

namespace iw {
inline namespace Editor {
	class App
		: public iw::Application
	{
	public:
	private:
		ToolLayer* toolbox;
		bool active;

	public:
		App();

		int Initialize(
			iw::InitOptions& options) override;

		bool HandleCommand(
			const Command& command) override;

		//void HandleCollision(
		//	Manifold& manifold,
		//	scalar dt) override;
	};
}
}
