#pragma once

#include "iw/renderer/Device.h"

namespace iw {
namespace RenderAPI {
	class GLDevice
		: public IDevice
	{
	public:
		IWRENDERER_API
		void DrawElements(
			MeshTopology topology,
			unsigned count,
			ptrdiff_t offset) override;

		IWRENDERER_API
		void Clear() override;

		IWRENDERER_API
		void SetViewport(
			int x,
			int y) override;

		// State changes

		IWRENDERER_API
		virtual void SetWireframe(
			bool wireframe) override;

		// Buffers

		IWRENDERER_API
		void DestroyBuffer(
			IBuffer* buffer) override;

		IWRENDERER_API
		void UpdateBuffer(
			IBuffer* buffer,
			const void* data,
			size_t size = 0,
			size_t offset = 0) override;

		//Index buffers

		IWRENDERER_API
		IIndexBuffer* CreateIndexBuffer(
			const void* data,
			size_t count,
			BufferIOType io = STATIC) override;

		IWRENDERER_API
		void SetIndexBuffer(
			IIndexBuffer* indexBuffer) override;

		//Vertex buffers

		IWRENDERER_API
		IVertexBuffer* CreateVertexBuffer(
			const void* data,
			size_t size,
			BufferIOType io = STATIC) override;

		IWRENDERER_API
		void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) override;

		// Uniform buffers

		IWRENDERER_API
		IUniformBuffer* CreateUniformBuffer(
			const void* data,
			size_t size,
			BufferIOType io = DYNAMIC) override;

		IWRENDERER_API
		void SetUniformBuffer(
			IUniformBuffer* vertexBuffer) override;

		//Vertex arrays

		IWRENDERER_API
		IVertexArray* CreateVertexArray() override;

		IWRENDERER_API
		void DestroyVertexArray(
			IVertexArray* vertexArray) override;

		IWRENDERER_API
		void SetVertexArray(
			IVertexArray* vertexArray) override;

		IWRENDERER_API
		void AddBufferToVertexArray(
			IVertexArray* vertexArray,
			IVertexBuffer* buffer,
			const VertexBufferLayout& layout,
			int index = -1) override; // if index is negitive the next slot gets used

		IWRENDERER_API
		void UpdateVertexArrayData(
			IVertexArray* vertexArray,
			size_t bufferIndex,
			const void* data,
			size_t size,
			size_t offset = 0) override;

		//Vertex shader

		IWRENDERER_API
		IVertexShader* CreateVertexShader(
			const char* source) override;

		IWRENDERER_API
		void DestroyVertexArray(
			IVertexShader* vertexShader) override;

		//Fragment shader

		IWRENDERER_API
		IFragmentShader* CreateFragmentShader(
			const char* source) override;

		IWRENDERER_API
		void DestroyFragmentShader(
			IFragmentShader* fragmentShader) override;

		//Geometry shader

		IWRENDERER_API
		IGeometryShader* CreateGeometryShader(
			const char* source) override;

		IWRENDERER_API
		void DestroyGeometryShader(
			IGeometryShader* geometryShader) override;

		//Compute shader

		IWRENDERER_API
		virtual IComputeShader* CreateComputeShader(
			const char* source) override;

		IWRENDERER_API
		virtual void DestroyComputeShader(
			IComputeShader* computeShader) override;

		//Shader pipeline

		IWRENDERER_API
		IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader) override;

		IWRENDERER_API
		void DestroyPipeline(
			IPipeline* pipeline) override;

		IWRENDERER_API
		void SetPipeline(
			IPipeline* pipeline) override;

		//Compute shader pipeline

		IWRENDERER_API
		virtual IComputePipeline* CreateComputePipeline(
			IComputeShader* computeShader) override;

		IWRENDERER_API
		virtual void DestroyComputePipeline(
			IComputePipeline* computePipeline) override;

		IWRENDERER_API
		virtual void SetComputePipeline(
			IComputePipeline* ComputePipeline) override;

		// Textures

		IWRENDERER_API
		virtual ITexture* CreateTexture(
			int width,
			int height,
			TextureType type,
			TextureFormat format,
			TextureFormatType formatType,
			TextureWrap wrap,
			const void* data = nullptr) override;

		IWRENDERER_API
		virtual ITexture* CreateSubTexture(
			ITexture* texture,
			int xOffset,
			int yOffset,
			int width,
			int height,
			int mipmap = 0) override;

		IWRENDERER_API
		virtual void DestroyTexture(
			ITexture* texture)  override;

		IWRENDERER_API
		virtual void SetTexture(
			ITexture* texture)  override;

		// Frame buffers

		IWRENDERER_API
		IFrameBuffer* CreateFrameBuffer(
			bool noColor) override;

		IWRENDERER_API
		void DestroyFrameBuffer(
			IFrameBuffer* frameBuffer) override;

		IWRENDERER_API
		void SetFrameBuffer(
			IFrameBuffer* frameBuffer) override;

		IWRENDERER_API
		void AttachTextureToFrameBuffer(
			IFrameBuffer* frameBuffer,
			ITexture* texture) override;
	};
}

	using namespace RenderAPI;
}
