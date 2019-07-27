#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/Pipeline.h"
#include "iw/util/set/sparse_set.h"

namespace IwRenderer {
	class IWRENDERER_API GLPipeline
		: public IPipeline
	{
	private:
		struct {
			iwutil::sparse_set<unsigned int, GLPipelineParam*> m_params;
		};

		unsigned int m_program;

	public:
		GLPipeline(
			GLVertexShader* vertexShader,
			GLFragmentShader* fragmentShader,
			GLGeometryShader* geometryShader);

		~GLPipeline();

		IPipelineParam* GetParam(
			const char* name);

		void Use() const;
	};
}
