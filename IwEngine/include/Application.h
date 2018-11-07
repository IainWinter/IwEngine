#pragma once

#include "iwengine.h"

namespace IwEngine {
	class IWENGINE_API Application {
	public:
		Application();
		virtual ~Application();
	private:
		virtual void Run();
	};
}