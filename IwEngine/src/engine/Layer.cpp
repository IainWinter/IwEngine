#include "iw/engine/Layer.h"

namespace IwEngine {
	Layer::Layer(
		const char* name)
		: m_name(name)
	{}

	Layer::~Layer() {}

	int Layer::Initialize() {
		return 0;
	}

	void Layer::Destroy() {}
	void Layer::Update() {}
	void Layer::FixedUpdate() {}
	void Layer::ImGui() {}

	bool Layer::On(
		WindowResizedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseWheelEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseMovedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		MouseButtonEvent& event)
	{
		return false;
	}

	bool Layer::On(
		KeyEvent& event)
	{
		return false;
	}

	bool Layer::On(
		KeyTypedEvent& event)
	{
		return false;
	}
}
