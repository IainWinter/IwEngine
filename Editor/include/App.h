#pragma once 

#include "iw/engine/EntryPoint.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/engine/Layer.h"

namespace iw {
namespace Editor {
	class App
		: public iw::Application
	{
	public:
	private:
		ImGuiLayer* imgui;

		Layer* toolbox;
		Layer* sandbox;

	public:
		App();

		int Initialize(
			iw::InitOptions& options) override;

		void Update() override;
		void FixedUpdate() override;

		bool HandleCommand(
			const Command& command) override;

		//void HandleCollision(
		//	Manifold& manifold,
		//	scalar dt) override;
	};
}
}
