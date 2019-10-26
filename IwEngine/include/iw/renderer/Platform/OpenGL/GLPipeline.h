#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/Pipeline.h"
#include <unordered_map>

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLPipeline
		: public IPipeline
	{
	private:
		struct {
			std::unordered_map<const char*, GLPipelineParam*> m_params;
		};

		unsigned int m_bufferCount;
		unsigned int m_programId;

	public:
		GLPipeline(
			GLVertexShader* vertexShader,
			GLFragmentShader* fragmentShader,
			GLGeometryShader* geometryShader);

		~GLPipeline();

		IPipelineParam* GetParam(
			const char* name);

		void SetBuffer(
			const char* name,
			IUniformBuffer* buffer);

		void Use() const;
	};
}
}
