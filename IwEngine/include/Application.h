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
	protected:
		Window& window;

	public:
		Application();
		virtual ~Application();

		void PushLayer(
			Layer* layer);

		void PushOverlay(
			Layer* overlay);

		void PopLayer(
			Layer* layer);

		void PopOverlay(
			Layer* overlay);

		virtual int Initilize(
			WindowOptions& windowOptions);

		virtual void Start();
		virtual void Stop();
		virtual void Destroy();

		virtual void OnEvent(
			Event& e);
	private:
		void Run();
	};

	Application* CreateApplication();
}
