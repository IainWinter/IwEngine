#pragma once

#include "GLVertexShader.h"
#include "GLFragmentShader.h"
#include "GLGeometryShader.h"
#include "GlPipelineParam.h"
#include "iw/renderer/Pipeline.h"
#include <unordered_map>
#include <string>

namespace iw {
namespace RenderAPI {
	class GLPipeline
		: public IPipeline
	{
	private:
		std::unordered_map<std::string, GLPipelineParam*> m_params;

		unsigned gl_id;
		unsigned m_bufferCount;
		unsigned m_textureCount;

	public:
		IWRENDERER_API
		GLPipeline(
			GLVertexShader* vertexShader,
			GLFragmentShader* fragmentShader,
			GLGeometryShader* geometryShader);

		IWRENDERER_API
		~GLPipeline();

		IWRENDERER_API
		int UniformCount() const override;

		IWRENDERER_API
		int BufferCount() const override;


		IWRENDERER_API
		unsigned Handle() const override;

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
		void Use();
	};
}

	using namespace RenderAPI;
}
