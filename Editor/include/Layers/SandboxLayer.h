#pragma once 

#include "iw/engine/Layer.h"
#include "iw/physics/Dynamics/DynamicsSpace.h"

namespace IW {
	class SandboxLayer
		: public Layer
	{
	private:
		iw::vector3 movement; // this is going to be editing the toolbox camera in the future so wont be here
		DynamicsSpace space;
	public:
		SandboxLayer();

		int Initialize() override;
		void PostUpdate() override;
		void FixedUpdate() override;

		bool On(ActionEvent& e) override;
	};
}
