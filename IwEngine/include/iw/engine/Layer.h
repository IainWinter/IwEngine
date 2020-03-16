#pragma once

#include "Core.h"
#include "EventStack.h"
#include "System.h"
#include "InitOptions.h"
#include "Events/Events.h"
#include "iw/entity/Space.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/asset/AssetManager.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"
#include "iw/audio/AudioSpace.h"
#include "iw/events/eventbus.h"

#undef PlaySound

namespace iw {
namespace Engine {
	class Application;

	class Layer {
	private:
		const char* m_name;
		EventStack<ISystem*> m_systems; // layer doesnt own systems but prolly should
	protected:
		iw::ref<Space>          Space;
		iw::ref<iw::QueuedRenderer> Renderer;
		iw::ref<AssetManager>   Asset;
		iw::ref<DynamicsSpace>  Physics;
		iw::ref<AudioSpace>     Audio;
		iw::ref<iw::eventbus>   Bus;

	public:
		IWENGINE_API
		Layer(
			const char* name);

		IWENGINE_API
		virtual ~Layer();

		IWENGINE_API
		virtual int Initialize();

		// Sync Updates

		IWENGINE_API
		virtual void Destroy();

		IWENGINE_API
		virtual void ImGui();

		IWENGINE_API
		virtual void PreUpdate();

		IWENGINE_API
		virtual void Update();

		IWENGINE_API
		virtual void PostUpdate();

		IWENGINE_API
		virtual void FixedUpdate();

		// Action events

		IWENGINE_API
		virtual bool On(
			ActionEvent& e);

		// Input events

		IWENGINE_API
		virtual bool On(
			MouseWheelEvent& e);

		IWENGINE_API
		virtual bool On(
			MouseMovedEvent& e);

		IWENGINE_API
		virtual bool On(
			MouseButtonEvent& e);

		IWENGINE_API
		virtual bool On(
			KeyEvent& e);

		IWENGINE_API
		virtual bool On(
			KeyTypedEvent& e);

		// Window events

		IWENGINE_API
		virtual bool On(
			WindowResizedEvent& e);

		// Physics events

		IWENGINE_API
		virtual bool On(
			CollisionEvent& e);

		// Entity events

		IWENGINE_API
		virtual bool On(
			EntityDestroyedEvent& e);

		// System updates

		IWENGINE_API
		void UpdateSystems();

		IWENGINE_API
		void FixedUpdateSystems();

		inline const char* Name() {
			return m_name;
		}

		template<
			typename S,
			typename... Args>
		S* PushSystem(
			Args&&... args)
		{
			S* layer = new S(std::forward<Args>(args)...);
			layer->SetLayerVars(Space, Renderer, Asset, Physics, Audio, Bus);

			m_systems.PushBack(layer);
			return layer;
		}

		template<
			typename S>
		void PopSystem(
			const char* name)
		{
			auto itr = m_systems.begin();
			for (; itr != m_systems.end(); itr++;) {
				if (strcmp((*itr)->Name(), name)) {
					break;
				}
			}

			if (itr != m_systems.end()) {
				m_systems.erase(itr);
			}
		}
	private:
		friend class Application;

		IWENTITY_API
		void SetApplicationVars(
			iw::ref<iw::Space> space,
			iw::ref<iw::QueuedRenderer> renderer,
			iw::ref<AssetManager> asset,
			iw::ref<DynamicsSpace> physics,
			iw::ref<AudioSpace> audio,
			iw::ref<iw::eventbus> bus);
	};
}

	using namespace Engine;
}
