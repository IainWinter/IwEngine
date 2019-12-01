#pragma once

#include "iw/renderer/Device.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLDevice
		: public IDevice
	{
	public:
		void DrawElements(
			MeshTopology topology,
			unsigned count,
			ptrdiff_t offset) override;

		void Clear() override;

		void SetViewport(
			int x,
			int y) override;

		//Index buffers

		IIndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyIndexBuffer(
			IIndexBuffer* indexBuffer) override;

		void SetIndexBuffer(
			IIndexBuffer* indexBuffer) override;

		//Vertex buffers

		IVertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyVertexBuffer(
			IVertexBuffer* vertexBuffer) override;

		void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) override;

		void UpdateVertexBufferData(
			IVertexBuffer* buffer,
			const void* data,
			size_t size = 0) override;

		// Uniform buffers

		IUniformBuffer* CreateUniformBuffer(
			size_t size,
			const void* data = nullptr) override;

		void DestroyUniformBuffer(
			IUniformBuffer* vertexBuffer) override;

		void SetUniformBuffer(
			IUniformBuffer* vertexBuffer) override;

		void UpdateUniformBufferData(
			IUniformBuffer* uniformBuffer,
			const void* data = nullptr) override;

		//Vertex arrays

		IVertexArray* CreateVertexArray() override;

		void DestroyVertexArray(
			IVertexArray* vertexArray) override;

		void SetVertexArray(
			IVertexArray* vertexArray) override;

		void AddBufferToVertexArray(
			IVertexArray* vertexArray,
			IVertexBuffer* buffer,
			const VertexBufferLayout& layout) override;

		void UpdateVertexArrayData(
			IVertexArray* vertexArray,
			size_t bufferIndex,
			const void* data,
			size_t size = 0) override;

		//Vertex shader

		IVertexShader* CreateVertexShader(
			const char* source) override;

		void DestroyVertexArray(
			IVertexShader* vertexShader) override;

		//Fragment shader

		IFragmentShader* CreateFragmentShader(
			const char* source) override;

		void DestroyFragmentShader(
			IFragmentShader* fragmentShader) override;

		//Geometry shader

		IGeometryShader* CreateGeometryShader(
			const char* source) override;

		void DestroyGeometryShader(
			IGeometryShader* geometryShader) override;

		//Compute shader

		virtual IComputeShader* CreateComputeShader(
			const char* source) override;

		virtual void DestroyComputeShader(
			IComputeShader* computeShader) override;

		//Shader pipeline

		IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader) override;

		void DestroyPipeline(
			IPipeline* pipeline) override;

		void SetPipeline(
			IPipeline* pipeline) override;

		//Compute shader pipeline

		virtual IComputePipeline* CreateComputePipeline(
			IComputeShader* computeShader) override;

		virtual void DestroyComputePipeline(
			IComputePipeline* computePipeline) override;

		virtual void SetComputePipeline(
			IComputePipeline* ComputePipeline) override;

		// Textures

		virtual ITexture* CreateTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			const void* data = nullptr) override;

		virtual void DestroyTexture(
			ITexture* texture)  override;

		virtual void SetTexture(
			ITexture* texture)  override;

		// Frame buffers

		IFrameBuffer* CreateFrameBuffer() override;

		void DestroyFrameBuffer(
			IFrameBuffer* frameBuffer) override;

		void SetFrameBuffer(
			IFrameBuffer* frameBuffer) override;

		void AttachTextureToFrameBuffer(
			IFrameBuffer* frameBuffer,
			ITexture* texture) override;
	};
}
}
