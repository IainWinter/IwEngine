#include "iw/input/Context.h"

namespace IW {
	Context::Context(
		std::string name, 
		float width, 
		float height)
		: Name(name)
		, Width(width)
		, Height(height)
	{}

	void Context::AddDevice(
		const iw::ref<Device>& device)
	{
		Devices.push_back(device);
	}

	void Context::RemoveDevice(
		const iw::ref<Device>& device)
	{
		auto itr = std::find(Devices.begin(), Devices.end(), device);
		if (itr != Devices.end()) {
			Devices.erase(itr);
		}
	}
}
