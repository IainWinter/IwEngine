#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
#include "iw/util/memory/ref.h"
#include "iw/util/jobs/node.h"
#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Model.h"
#include "iw/engine/Components/CameraController.h"

namespace iw {
	struct Render
		: iw::node
	{
		iw::ref<Renderer> renderer;
		iw::ref<Space>    space;

		Render(
			iw::ref<Renderer> r,
			iw::ref<Space> s);

		void SetLight(
			DirectionalLight& light);

		iw::ref<Texture>& GetTexture();
		Light* GetLight();

		void execute() override;
	};
}
