#include "iw/engine/Layers/DebugLayer.h"
#include "iw/common/Components/Transform.h"
#include "iw/engine/Time.h"
#include "imgui/imgui.h"
#include <gl\glew.h>

namespace iw {
namespace Engine {
	DebugLayer::DebugLayer()
		: Layer("Debug")
	{}

	void DebugLayer::ImGui() {
		while (logs.size() > 20) {
			logs.erase(logs.begin());
		}

		ImGui::Begin("Debug layer");

		float time = Time::FixedTime();
		ImGui::SliderFloat("Fixed timestep", &time, 0, 1);

		if (time != Time::FixedTime()) {
			Time::SetFixedTime(time);
		}

		ImGui::Text("Last 20 Events");
		for (std::string log : logs) {
			ImGui::Text(log.c_str());
		}

		ImGui::End();
	}

	bool DebugLayer::On(
		MouseWheelEvent& e)
	{
		std::stringstream log;
		log << "Mouse wheel .... delta: " << e.Delta << " Name: " << iw::val(e.Device);
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		MouseMovedEvent& e)
	{
		std::stringstream log;
		log << "Mouse moved .... X: " << e.X << " Y: " << e.Y << " dX: " << e.DeltaX << " dY: " << e.DeltaY << " Name: " << iw::val(e.Device);
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		MouseButtonEvent& e)
	{
		std::stringstream log;
		log << "Mouse button ... Button: " << e.Button << " State: " << e.State << " Name: " << iw::val(e.Device);
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		KeyEvent& e)
	{
		std::stringstream log;
		log << "Key button ..... Button: " << e.Button << " State: " << e.State << " Name: " << iw::val(e.Device);
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		KeyTypedEvent& e)
	{
		std::stringstream log;
		log << "Key types ...... Button: " << e.Button << " Char: " << e.Character << " Name: " << iw::val(e.Device);
		logs.push_back(log.str());
		return false;
	}

	bool DebugLayer::On(
		WindowResizedEvent& e)
	{
		std::stringstream log;
		log << "Window resized . W: " << e.Width << " H: " << e.Height;
		logs.push_back(log.str());
		return false;
	}
}
}
