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

		Device* SetIndexBuffer(
			IndexBuffer* indexBuffer) override;

		//Vertex buffers
		VertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyVertexBuffer(
			VertexBuffer* vertexBuffer) override;

		Device* SetVertexBuffer(
			VertexBuffer* vertexBuffer) override;

		//Vertex arrays
		VertexArray* CreateVertexArray(
			size_t numBuffers,
			VertexBuffer** vertexBuffers,
			VertexBufferLayout** vertexLayouts) override;

		void DestroyVertexArray(
			VertexArray* vertexArray) override;

		Device* SetVertexArray(
			VertexArray* vertexArray) override;
	};
}
