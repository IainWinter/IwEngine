#include "iw/engine/Layer.h"

namespace IW {
	Layer::Layer(
		IW::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset,
		const char* name)
		: m_name(name)
		, Space(space)
		, Renderer(renderer)
		, Asset(asset)
	{}

	Layer::~Layer() {}

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

	void Layer::PreUpdate() {}
	void Layer::Update() {}
	void Layer::PostUpdate() {}
	void Layer::FixedUpdate() {}
	void Layer::ImGui() {}

	bool Layer::On(
		IW::WindowResizedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		IW::MouseWheelEvent& event)
	{
		return false;
	}

	bool Layer::On(
		IW::MouseMovedEvent& event)
	{
		return false;
	}

	bool Layer::On(
		IW::MouseButtonEvent& event)
	{
		return false;
	}

	bool Layer::On(
		IW::KeyEvent& event)
	{
		return false;
	}

	bool Layer::On(
		IW::KeyTypedEvent& event)
	{
		return false;
	}

	void Layer::UpdateSystems() {
		for (ISystem* system : m_systems) {
			system->Update();
		}
	}

	void Layer::FixedUpdateSystems() {
		for (ISystem* system : m_systems) {
			system->FixedUpdate();
		}
	}
}
