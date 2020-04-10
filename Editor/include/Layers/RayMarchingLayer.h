#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"

namespace iw {
	class RayMarchingLayer
		: public Layer
	{
	private:
		ref<Shader> shader;
		Camera*     camera;

	public:
		RayMarchingLayer();

		int Initialize() override;

		void PostUpdate() override;

		void ImGui() override;
	};
}
