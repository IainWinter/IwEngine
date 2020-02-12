#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
#include "iw/util/memory/ref.h"
#include "iw/util/jobs/node.h"
#include "iw/graphics/Model.h"

namespace iw {
	struct FilterTarget
		: iw::node
	{
		iw::ref<Renderer> renderer;

		FilterTarget(
			iw::ref<Renderer> r);

		void SetIntermediate(
			iw::ref<RenderTarget>& target);

		void SetShader(
			iw::ref<Shader>& shader);

		void SetBlur(
			iw::vector2& blur);

		iw::ref<RenderTarget>& GetTarget();
		iw::ref<RenderTarget>& GetIntermediate();
		iw::ref<Shader>& GetShader();
		iw::vector2& GetBlur();

		void execute() override;
	};

}
