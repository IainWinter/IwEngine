#pragma once

#include "Core.h"

namespace IwEngine {
	class IWENGINE_API Application {
	public:
		Application();
		virtual ~Application();

		virtual void Run();
	};

	//Extern in client
	Application* CreateApplication();
}