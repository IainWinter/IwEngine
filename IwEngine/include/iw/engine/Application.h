#pragma once

#include "Core.h"
#include "Window.h"
#include "LayerStack.h"
#include "iw/input/InputManager.h"
#include "ImGui/ImGuiLayer.h"

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		IWindow*  m_window;
		LayerStack m_layerStack;
		ImGuiLayer* m_imguiLayer;
		IwInput::InputManager m_inputManager;
		bool m_running;

	public:
		Application();
		virtual ~Application();

		virtual int Initialize(
			const WindowOptions& windowOptions);

		virtual void Run();
		virtual void Destroy();
		virtual void Update();

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

IwEngine::Application* CreateApplication();
