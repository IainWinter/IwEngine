#include "iw/engine/Layer.h"

namespace IwEngine {
	Layer::Layer(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue,
		const char* name)
		: m_name(name)
		, Space(space)
		, RenderQueue(renderQueue)
	{}

	Layer::~Layer() {}

	void Layer::UpdateSystems() {
		for (ISystem* system : m_systems) {
			system->Update();
		}
	}

	int Layer::Initialize(
		InitOptions& options) 
	{
		for (ISystem* s : m_systems) {
			int e = s->Initialize();
			if (e != 0) return e;
		}

		return 0;
	}

	void Layer::Destroy() {
		for (ISystem* s : m_systems) {
			s->Destroy();
		}
	}

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
