#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"

namespace iw {
	class SimpleSandLayer
		: public Layer
	{
	private:
		ref<Shader> shader;
		Camera*     camera;
		iw::ref<RenderTarget> target;
		iw::ref<Texture> texture;

	public:
		SimpleSandLayer();

		int Initialize() override;
		void PostUpdate() override;
	};
}
