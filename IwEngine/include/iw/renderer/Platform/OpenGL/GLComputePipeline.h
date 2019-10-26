#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GLComputeShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/ComputePipeline.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLComputePipeline
		: public IComputePipeline
	{
	private:
		unsigned int m_program;

	public:
		GLComputePipeline(
			GLComputeShader* computeShader);

		~GLComputePipeline();

		void DispatchComputeShader(
			int x,
			int y,
			int z) override;

		void Use() const;
	};
}
}
