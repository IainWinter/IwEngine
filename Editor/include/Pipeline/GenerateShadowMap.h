#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/QueuedRenderer.h"
#include "iw/util/memory/ref.h"
#include "iw/util/jobs/node.h"
#include "iw/graphics/DirectionalLight.h"
#include "iw/graphics/Model.h"

namespace iw {
	struct GenerateShadowMap
		: iw::node
	{
		iw::ref<Renderer> renderer;
		iw::ref<Space>    space;

		GenerateShadowMap(
			iw::ref<Renderer> r,
			iw::ref<Space> s);

		void SetLight(
			DirectionalLight& light);

		void SetThreshold(
			float threshold);

		Light* GetLight();

		float& GetThreshold();

		void execute() override;
	};
}
