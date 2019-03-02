#pragma once

#include "Core.h"
#include "Window.h"

namespace IwEngine {
	class IWENGINE_API Application {
	private:
		Window* m_window;
	protected:
		Window& window;

	public:
		Application();
		virtual ~Application();

		virtual void Initilize();
		virtual void Start();
		virtual void Stop();
		virtual void Destroy();

		virtual void Run() = 0;
	};

	Application* CreateApplication();
}
