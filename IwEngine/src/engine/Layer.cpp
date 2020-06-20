#include "iw/engine/Layer.h"

#include "iw/engine/Time.h"
#include <unordered_map>

namespace iw {
namespace Engine {
	Layer::Layer(
		const char* name)
		: m_name(name)
		, MainScene(new Scene())
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

	void Layer::OnPush() {}
	void Layer::OnPop() {}

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

	bool Layer::On(
		OsEvent& e)
	{
		return false;
	}

	bool Layer::On(
		CollisionEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		EntityDestroyEvent& e)
	{
		return m_systems.On(e);
	}

	bool Layer::On(
		EntityDestroyedEvent& e)
	{
		return m_systems.On(e);
	}

	void Layer::UpdateSystems(std::unordered_map<const char*, float>& temp_debug, float smooth) {
		for (ISystem* system : m_systems) {
			float start = iw::Time::DeltaTimeNow();
				
			system->Update();
				
			float end = iw::Time::DeltaTimeNow();
			temp_debug[system->Name()] = iw::lerp(temp_debug[system->Name()], end - start, smooth);
		}
	}

	void Layer::FixedUpdateSystems() {
		for (ISystem* system : m_systems) {
			system->FixedUpdate();
		}
	}
}
}
