#include "iw/engine/Layer.h"

#include "iw/engine/Time.h"
#include <unordered_map>

#include "iw/log/logger.h"

namespace iw {
namespace Engine {
	Layer::Layer(
		std::string name)
		: m_name(name)
		, MainScene(new Scene())
	{}

	Layer::~Layer() {
		delete MainScene;
	}

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

	void Layer::ImGui() {
		for (ISystem* s : m_systems) {
			s->ImGui();
		}
	}

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

	void Layer::UpdateSystems() {
		for (ISystem* system : m_systems) {
			LOG_TIME_SCOPE(system->Name() + " system");
			system->Update();
		}
	}

	void Layer::FixedUpdateSystems() {
		for (ISystem* system : m_systems) {
			system->FixedUpdate();
		}
	}
}
}
