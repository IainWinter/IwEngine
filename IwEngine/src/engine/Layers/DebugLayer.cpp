#include "iw/engine/Layers/DebugLayer.h"
#include "iw/engine/Components/Log.h"
#include "iw/common/Components/Transform.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include <gl\glew.h>

namespace IW {
	DebugLayer::DebugLayer()
		: Layer("Debug")
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
		IW::MouseWheelEvent& e)
	{
		std::stringstream log;
		log << "Mouse wheel .... delta: " << e.Delta;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::MouseMovedEvent& e)
	{
		std::stringstream log;
		log << "Mouse moved .... X: " << e.X << " Y: " << e.Y << " dX: " << e.DeltaX << " dY : " << e.DeltaY;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::MouseButtonEvent& e)
	{
		std::stringstream log;
		log << "Mouse button ... Button: " << e.Button << " State: " << e.State;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::KeyEvent& e)
	{
		std::stringstream log;
		log << "Key button ..... Button: " << e.Button << " State: " << e.State;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::KeyTypedEvent& e)
	{
		std::stringstream log;
		log << "Key types ...... Button: " << e.Button << " Char: " << e.Character;
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		IW::WindowResizedEvent& e)
	{
		std::stringstream log;
		log << "Window resized . W: " << e.Width << " H: " << e.Height;
		logs.push_back(log.str());
		return false;
	}
}
