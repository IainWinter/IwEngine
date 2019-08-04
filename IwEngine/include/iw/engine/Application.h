#pragma once

#include "Core.h"
#include "Window.h"
#include "Stack.h"
#include "InitOptions.h"
#include "Layers/ImGuiLayer.h"
#include "iw/input/InputManager.h"
#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include <vector>

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		bool                 m_running;
		IWindow*             m_window;
		ImGuiLayer*          m_imguiLayer;
		Stack<Layer*>        m_layers;
		IwRenderer::IDevice* m_device;

	protected:
		IwEntity::Space         Space;
		IwInput::InputManager   InputManager;
		IwGraphics::RenderQueue RenderQueue;

	public:
		Application();
		virtual ~Application();

		virtual int Initialize(
			InitOptions& options);

		virtual void Run();
		virtual void Destroy();
		virtual void Update();
		virtual void FixedUpdate();

		virtual void HandleEvent(
			Event& e);

		inline IWindow& GetWindow() {
			return *m_window;
		}

		template<
			typename L,
			typename... Args>
		L* PushLayer(
			Args&&... args)
		{
			L* layer = new L(Space, RenderQueue, std::forward<Args>(args)...);
			m_layers.PushBack(layer);
			return layer;
		}

		template<
			typename L,
			typename... Args>
		L* PushOverlay(
				Args&& ... args)
		{
			L* layer = new L(Space, RenderQueue, std::forward<Args>(args)...);
			m_layers.PushFront(layer);
			return layer;
		}

		template<
			typename L>
		void PopLayer(
			L* layer)
		{
			m_layers.Pop(layer);
		}
	private:
		template<
			typename _event_t>
		void DispatchEvent(
			_event_t& event)
		{
			if constexpr (std::is_same_v<WindowResizedEvent, _event_t>) {
				m_window->SetDimensions(event.Width, event.Height);
			}

			for (Layer* layer : m_layers) {
				if (layer->On(event)) {
					LOG_INFO << "Event handled by " << layer->Name() << " layer";
					event.Handled = true;
					break;
				}
			}
		}
	};
}

IwEngine::Application* CreateApplication(
	IwEngine::InitOptions& options);
