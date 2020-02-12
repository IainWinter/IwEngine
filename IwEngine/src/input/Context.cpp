#include "iw/input/Context.h"

namespace iw {
namespace Input {
	Context::Context(
		std::string name, 
		float width, 
		float height)
		: Name(name)
		, Width(width)
		, Height(height)
	{}

	void Context::AddDevice(
		iw::ref<Device>& device)
	{
		m_devices.emplace_back(device);
	}

	void Context::RemoveDevice(
		iw::ref<Device>& device)
	{
		auto itr = std::find(m_devices.begin(), m_devices.end(), device);
		if (itr != m_devices.end()) {
			m_devices.erase(itr);
		}
	}

	void Context::MapButton(
		InputName input,
		std::string command)
	{
		m_commands[input] = command;
	}
}
}
