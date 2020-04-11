#pragma once 

#include "iw/engine/EntryPoint.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "Layers/ToolLayer.h"
#include "Layers/SandboxLayer.h"

namespace iw {
namespace Editor {
	class App
		: public iw::Application
	{
	public:
	private:
		ToolLayer*  toolbox;
		ImGuiLayer* imgui;
		SandboxLayer* sandbox;

	public:
		App();

		int Initialize(
			iw::InitOptions& options) override;

		void Update() override;

		bool HandleCommand(
			const Command& command) override;

		//void HandleCollision(
		//	Manifold& manifold,
		//	scalar dt) override;
	};
}
}
