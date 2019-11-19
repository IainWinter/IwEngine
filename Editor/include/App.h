#pragma once 

#include "iw/engine/EntryPoint.h"

namespace IW {
inline namespace Editor {
	class App
		: public IW::Application
	{
	public:
	private:

	public:
		App();

		int Initialize(
			IW::InitOptions& options) override;
	private:
	};
}
}
