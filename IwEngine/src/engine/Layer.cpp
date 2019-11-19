#include "iw/engine/Layer.h"

namespace IW {
	Layer::Layer(
		const char* name)
		: m_name(name)
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
		 return m_systems.On(event);
	}

	bool Layer::On(
		IW::MouseWheelEvent& event)
	{
		return m_systems.On(event);
	}

	bool Layer::On(
		IW::MouseMovedEvent& event)
	{
		return m_systems.On(event);
	}

	bool Layer::On(
		IW::MouseButtonEvent& event)
	{
		return m_systems.On(event);
	}

	bool Layer::On(
		IW::KeyEvent& event)
	{
		return m_systems.On(event);
	}

	bool Layer::On(
		IW::KeyTypedEvent& event)
	{
		return m_systems.On(event);
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

	void Layer::SetApplicationVars(
		iw::ref<IW::Space> space,
		iw::ref<IW::Renderer> renderer,
		iw::ref<IW::AssetManager> asset,
		iw::ref<iw::eventbus> bus)
	{
		Space    = space;
		Renderer = renderer;
		Asset    = asset;
		Bus      = bus;
	}
}
