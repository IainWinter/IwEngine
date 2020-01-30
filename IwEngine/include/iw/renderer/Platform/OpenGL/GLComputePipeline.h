#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GLComputeShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/ComputePipeline.h"

namespace IW {
namespace RenderAPI {
	class GLComputePipeline
		: public IComputePipeline
	{
	private:
		unsigned gl_id;

	public:
		IWRENDERER_API
		GLComputePipeline(
			GLComputeShader* computeShader);

		IWRENDERER_API
		~GLComputePipeline();

		IWRENDERER_API
		void DispatchComputeShader(
			int x,
			int y,
			int z) override;

		IWRENDERER_API
		void Use() const;
	};
}

	using namespace RenderAPI;
}
