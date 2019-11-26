#pragma once

#include "Core.h"
#include "EventStack.h"
#include "System.h"
#include "InitOptions.h"
#include "Events/Events.h"
#include "iw/asset/AssetManager.h"
#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
#include "iw/events/eventbus.h"

namespace IW {
inline namespace Engine {
	class Application;

	class IWENGINE_API Layer {
	private:
		const char* m_name;
		EventStack<ISystem*> m_systems; // layer doesnt own systems but prolly should
	protected:
		iw::ref<IW::Space>        Space;
		iw::ref<IW::Renderer>     Renderer;
		iw::ref<IW::AssetManager> Asset;
		iw::ref<iw::eventbus>     Bus;

	public:
		Layer(
			const char* name);

		virtual ~Layer();

		virtual int Initialize();

		// Sync Updates

		virtual void Destroy();
		virtual void ImGui();
		virtual void PreUpdate();
		virtual void Update();
		virtual void PostUpdate();
		virtual void FixedUpdate();


		// Input events

		virtual bool On(IW::MouseWheelEvent&    e);
		virtual bool On(IW::MouseMovedEvent&    e);
		virtual bool On(IW::MouseButtonEvent&   e);
		virtual bool On(IW::KeyEvent&           e);
		virtual bool On(IW::KeyTypedEvent&      e);

		// Window events

		virtual bool On(IW::WindowResizedEvent& e);

		// System updates

		void UpdateSystems();
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
			layer->SetLayerVars(Space, Renderer);

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

		void SetApplicationVars(
			iw::ref<IW::Space> space,
			iw::ref<IW::Renderer> renderer,
			iw::ref<IW::AssetManager> asset,
			iw::ref<iw::eventbus> bus);
	};
}
}
