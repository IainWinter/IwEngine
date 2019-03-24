#pragma once

#include "iw/renderer/Device.h"

namespace IwRenderer {
	class IWRENDERER_API GLDevice
		: public Device
	{
	public:
		void DrawElements(
			int count,
			long long offset) override;

		//Index buffers
		IndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyIndexBuffer(
			IndexBuffer* indexBuffer) override;

		void SetIndexBuffer(
			IndexBuffer* indexBuffer) override;

		//Vertex buffers
		VertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyVertexBuffer(
			VertexBuffer* vertexBuffer) override;

		void SetVertexBuffer(
			VertexBuffer* vertexBuffer) override;

		//Vertex arrays
		VertexArray* CreateVertexArray(
			size_t numBuffers,
			VertexBuffer** vertexBuffers,
			VertexBufferLayout** vertexLayouts) override;

		void DestroyVertexArray(
			VertexArray* vertexArray) override;

		void SetVertexArray(
			VertexArray* vertexArray) override;

		//Vertex shader
		VertexShader* CreateVertexShader(
			const char* source) override;

		void DestroyVertexArray(
			VertexShader* vertexShader) override;

		//Fragment shader
		FragmentShader* CreateFragmentShader(
			const char* source) override;

		void DestroyFragmentShader(
			FragmentShader* fragmentShader) override;

		//Shader pipeline
		Pipeline* CreatePipeline(
			VertexShader* vertexShader,
			FragmentShader* fragmentShader) override;

		void DestroyPipeline(
			Pipeline* pipeline) override;

		void SetPipeline(
			Pipeline* pipeline) override;
	};
}
