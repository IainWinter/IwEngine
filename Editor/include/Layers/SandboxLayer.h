#pragma once 

#include "iw/engine/Layer.h"

namespace IW {
	class SandboxLayer
		: public Layer
	{
	private:
		iw::vector3 movement; // this is going to be editing the toolbox camera in the future so wont be here

	public:
		SandboxLayer();

		void PostUpdate() override;
	};
}
