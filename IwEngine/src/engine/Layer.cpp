#include "iw/engine/Layer.h"

namespace IW {
	Layer::Layer(
		const char* name)
		: m_name(name)
	{}

	Layer::~Layer() {}

	int Layer::Initialize() {
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
		ActionEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		MouseWheelEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		MouseMovedEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		MouseButtonEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		KeyEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		KeyTypedEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		WindowResizedEvent& e)
	{
		return m_systems.On(e);
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
		iw::ref<AssetManager> asset,
		iw::ref<DynamicsSpace> physics,
		iw::ref<iw::eventbus> bus)
	{
		Space    = space;
		Renderer = renderer;
		Asset    = asset;
		Physics  = physics;
		Bus      = bus;
	}
}
