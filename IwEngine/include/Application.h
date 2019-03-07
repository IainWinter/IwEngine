#pragma once

#include "Core.h"
#include "Window.h"
#include "Space.h"
#include "LayerStack.h"

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		Window* m_window;
		bool m_running;
		LayerStack m_layerStack;

	public:
		Application();
		virtual ~Application();

		virtual int Initilize(
			WindowOptions& windowOptions);

		virtual void Run();
		virtual void Destroy();
		virtual void Update();

		virtual void OnEvent(
			Event& e);

		void PushLayer(
			Layer* layer);

		void PushOverlay(
			Layer* overlay);

		void PopLayer(
			Layer* layer);

		void PopOverlay(
			Layer* overlay);

		inline Window& GetWindow() {
			return *m_window;
		}
	private:
		void InitilizeLayer(
			Layer* layer);

		void DestroyLayer(
			Layer* layer);
	};
}

IwEngine::Application* CreateApplication();
