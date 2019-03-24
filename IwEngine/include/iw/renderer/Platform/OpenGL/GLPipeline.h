#pragma once

#include "GLVertexShader.h"
#include "GlPipelineParam.h"
#include "GLFragmentShader.h"
#include "iw/renderer/Pipeline.h"
#include "iw/util/set/sparse_set.h"

namespace IwRenderer {
	class IWRENDERER_API GLPipeline
		: public Pipeline
	{
	private:
		unsigned int m_program;
		iwutil::sparse_set<unsigned int, GLPipelineParam*> m_params;

	public:
		GLPipeline(
			GLVertexShader* vertexShader,
			GLFragmentShader* fragmentShader);

		PipelineParam* GetParam(
			const char* name);

		void Use() const;
	};
}
