#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "InitOptions.h"
#include "iw/input/InputManager.h"
#include "iw/entity/Space.h"
#include "ImGui/ImGuiLayer.h"

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		IWindow*  m_window;
		LayerStack m_layerStack;
		ImGuiLayer* m_imguiLayer;
		bool m_running;

	protected:
		IwInput::InputManager InputManager;
		IwEntity::Space Space;

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

		void PushLayer(
			Layer* layer);

		void PushOverlay(
			Layer* overlay);

		void PopLayer(
			Layer* layer);

		void PopOverlay(
			Layer* overlay);

		inline IWindow& GetWindow() {
			return *m_window;
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

			for (Layer* layer : m_layerStack) {
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
