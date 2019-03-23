#pragma once

#include "IwRenderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"

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

		virtual Device* SetIndexBuffer(
			IndexBuffer* indexBuffer) = 0;

		//Vertex buffers
		virtual VertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyVertexBuffer(
			VertexBuffer* vertexBuffer) = 0;

		virtual Device* SetVertexBuffer(
			VertexBuffer* vertexBuffer) = 0;

		//Vertex arrays
		virtual VertexArray* CreateVertexArray(
			size_t numBuffers,
			VertexBuffer** vertexBuffers,
			VertexBufferLayout** vertexLayouts) = 0;

		virtual void DestroyVertexArray(
			VertexArray* vertexArray) = 0;

		virtual Device* SetVertexArray(
			VertexArray* vertexArray) = 0;
	};
}
