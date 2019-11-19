#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Components/Log.h"
#include "iw/data/Components/Transform.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include <gl\glew.h>

namespace IW {
	DebugLayer::DebugLayer(
		IW::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset,
		iw::eventbus& bus)
		: Layer(space, renderer, asset, bus, "Debug")
	{}

	struct Components {
		IW::Transform* Transform;
		VectorLog* Log;
	};

	void DebugLayer::PostUpdate() {
		//for (auto entity : Space.Query<IW::Transform, VectorLog>()) {
		//	auto [transform, log] = entity.Components.Tie<Components>();
		//}

		while (logs.size() > 20) {
			logs.erase(logs.begin());
		}
	}

	void DebugLayer::ImGui() {
		ImGui::Begin("Debug layer");


		float time = IW::Time::FixedTime();
		ImGui::SliderFloat("Fixed timestep", &time, 0, 1);

		if (time != IW::Time::FixedTime()) {
			IW::Time::SetFixedTime(time);
		}

		ImGui::Text("Last 20 Events");
		for (std::string log : logs) {
			ImGui::Text(log.c_str());
		}

		ImGui::End();
	}

	bool DebugLayer::On(
		IW::MouseWheelEvent& event)
	{
		std::stringstream log;
		log << "Mouse wheel .... delta: " << event.Delta;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::MouseMovedEvent& event)
	{
		std::stringstream log;
		log << "Mouse moved .... X: " << event.X << " Y: " << event.Y << " dX: " << event.DeltaX << " dY : " << event.DeltaY;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::MouseButtonEvent& event)
	{
		std::stringstream log;
		log << "Mouse button ... Button: " << event.Button << " State: " << event.State;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::KeyEvent& event)
	{
		std::stringstream log;
		log << "Key button ..... Button: " << event.Button << " State: " << event.State;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::KeyTypedEvent& event)
	{
		std::stringstream log;
		log << "Key types ...... Button: " << event.Button << " Char: " << event.Character;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::WindowResizedEvent& event)
	{
		std::stringstream log;
		log << "Window resized . W: " << event.Width << " H: " << event.Height;
		logs.push_back(log.str());
		return false;
	}
}
