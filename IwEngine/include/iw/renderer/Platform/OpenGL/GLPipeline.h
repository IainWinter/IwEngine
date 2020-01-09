#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/Pipeline.h"
#include <unordered_map>

namespace IW {
namespace RenderAPI {
	class GLPipeline
		: public IPipeline
	{
	private:
		std::unordered_map<const char*, GLPipelineParam*> m_params;

		unsigned int m_bufferCount;
		unsigned int m_textureCount;
		unsigned int m_programId;

	public:
		IWRENDERER_API
		GLPipeline(
			GLVertexShader* vertexShader,
			GLFragmentShader* fragmentShader,
			GLGeometryShader* geometryShader);

		IWRENDERER_API
		~GLPipeline();

		IWRENDERER_API
		IPipelineParam* GetParam(
			const char* name) override;

		IWRENDERER_API
		IPipelineParam* GetParam(
			int index) override;

		IWRENDERER_API
		void SetBuffer(
			const char* name,
			IUniformBuffer* buffer) override;

		IWRENDERER_API
		int UniformCount() override;

		IWRENDERER_API
		void Use();
	};
}

	using namespace RenderAPI;
}
