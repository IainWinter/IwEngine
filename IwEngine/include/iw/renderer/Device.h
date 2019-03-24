#pragma once

#include "IwRenderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "Pipeline.h"

namespace IwRenderer {
	class IWRENDERER_API Device {
	public:
		virtual void DrawElements(
			int count,
			long long offset) = 0;

		//Index buffers
		virtual IndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyIndexBuffer(
			IndexBuffer* indexBuffer) = 0;

		virtual void SetIndexBuffer(
			IndexBuffer* indexBuffer) = 0;

		//Vertex buffers
		virtual VertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyVertexBuffer(
			VertexBuffer* vertexBuffer) = 0;

		virtual void SetVertexBuffer(
			VertexBuffer* vertexBuffer) = 0;

		//Vertex arrays
		virtual VertexArray* CreateVertexArray(
			size_t numBuffers,
			VertexBuffer** vertexBuffers,
			VertexBufferLayout** vertexLayouts) = 0;

		virtual void DestroyVertexArray(
			VertexArray* vertexArray) = 0;

		virtual void SetVertexArray(
			VertexArray* vertexArray) = 0;

		//Vertex shader
		virtual VertexShader* CreateVertexShader(
			const char* source) = 0;

		virtual void DestroyVertexArray(
			VertexShader* vertexShader) = 0;

		//Fragment shader
		virtual FragmentShader* CreateFragmentShader(
			const char* source) = 0;

		virtual void DestroyFragmentShader(
			FragmentShader* fragmentShader) = 0;

		//Shader pipeline
		virtual Pipeline* CreatePipeline(
			VertexShader* vertexShader,
			FragmentShader* fragmentShader) = 0;

		virtual void DestroyPipeline(
			Pipeline* pipeline) = 0;

		virtual void SetPipeline(
			Pipeline* pipeline) = 0;
	};
}
