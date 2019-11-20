#pragma once 

#include "iw/engine/Layer.h"

namespace IW {
	class SandboxLayer
		: public Layer
	{
	public:
		SandboxLayer();

		void PostUpdate() override;
	};
}
