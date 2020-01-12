#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/Pipeline.h"
#include <unordered_map>
#include <string>

namespace IW {
namespace RenderAPI {
	class GLPipeline
		: public IPipeline
	{
	private:
		std::unordered_map<std::string, GLPipelineParam*> m_params;

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
			std::string name) override;

		IWRENDERER_API
		IPipelineParam* GetParam(
			int index) override;

		IWRENDERER_API
		void SetBuffer(
			std::string name,
			IUniformBuffer* buffer) override;

		IWRENDERER_API
		int UniformCount() override;

		IWRENDERER_API
		void Use();
	};
}

	using namespace RenderAPI;
}
