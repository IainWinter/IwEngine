#pragma once

namespace IwEngine {
	class Window {
	public:
		virtual ~Window() {}

		virtual int Open() = 0;
		virtual void Close() = 0;

		static Window* Create();
	};
}
