#include "iw/engine/Layer.h"

namespace IwEngine {
	Layer::Layer(
		const char* name)
		: m_name(name)
	{}

	Layer::~Layer() {}

	int Layer::Initilize() {
		return 0;
	}

	void Layer::Destroy() {}
	void Layer::Update() {}

	bool Layer::On(
		WindowResizedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseMovedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseButtonPressedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseButtonReleasedEvent& event)
	{
		return false;
	}
}
