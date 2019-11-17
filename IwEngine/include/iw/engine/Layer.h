#pragma once

#include "Core.h"
#include "EventStack.h"
#include "System.h"
#include "InitOptions.h"
#include "Events/Events.h"
#include "iw/asset/AssetManager.h"
#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"

namespace IW {
	class IWENGINE_API Layer {
	private:
		const char* m_name;
		std::vector<ISystem*> m_systems; // layer doesnt own systems but prolly should
	protected:
		IW::Space&  Space;
		IW::Renderer&     Renderer;
		IW::AssetManager& Asset;

	public:
		Layer(
			IW::Space& space,
			IW::Renderer& renderer,
			IW::AssetManager& asset,
			const char* name);

		virtual ~Layer();

		virtual int Initialize(
			InitOptions& options);

		// Sync Updates

		virtual void Destroy();
		virtual void ImGui();
		virtual void PreUpdate();
		virtual void Update();
		virtual void PostUpdate();
		virtual void FixedUpdate();

		// Input events

		virtual bool On(IW::MouseWheelEvent&    event);
		virtual bool On(IW::MouseMovedEvent&    event);
		virtual bool On(IW::MouseButtonEvent&   event);
		virtual bool On(IW::KeyEvent&           event);
		virtual bool On(IW::KeyTypedEvent&      event);

		// Window events

		virtual bool On(IW::WindowResizedEvent& event);

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
			S* layer = new S(Space, Renderer, std::forward<Args>(args)...);
			m_systems.push_back(layer);
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
	};
}
