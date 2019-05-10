#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GLComputeShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/ComputePipeline.h"
#include "iw/util/set/sparse_set.h"

namespace IwRenderer {
	class IWRENDERER_API GLComputePipeline
		: public IComputePipeline
	{
	private:
		unsigned int m_program;
		iwutil::sparse_set<unsigned int, GLPipelineParam*> m_params;

	public:
		GLComputePipeline(
			GLComputeShader* computeShader);

		~GLComputePipeline();

		IPipelineParam* GetParam(
			const char* name);

		void DispatchComputeShader(
			int x,
			int y,
			int z) override;

		void Use() const;
	};
}
