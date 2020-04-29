#pragma once

#include "IwRenderer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "UniformBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "VertexShader.h"
#include "FragmentShader.h"
#include "GeometryShader.h"
#include "ComputeShader.h"
#include "Pipeline.h"
#include "ComputePipeline.h"
#include "Texture.h"
#include "FrameBuffer.h"

namespace iw {
namespace RenderAPI {
	class IDevice {
	public:
		IWRENDERER_API
		virtual void DrawElements(
			MeshTopology topology,
			unsigned count,
			ptrdiff_t offset) = 0;

		IWRENDERER_API
		virtual void DrawElementsInstanced(
			MeshTopology topology,
			unsigned count,
			ptrdiff_t offset,
			unsigned instanceCount) = 0;

		IWRENDERER_API
		virtual void Clear() = 0;

		// should be in frame buffer?
		IWRENDERER_API
		virtual void SetViewport(
			int x,
			int y) = 0;

		// State changes

		IWRENDERER_API
		virtual void SetWireframe(
			bool wireframe) = 0;

		IWRENDERER_API
		virtual void SetCullFace(
			CullFace cull) = 0;

		// Buffers

		IWRENDERER_API
		virtual void DestroyBuffer(
			IBuffer* buffer) = 0;

		IWRENDERER_API
		virtual void UpdateBuffer(
			IBuffer* buffer,
			const void* data,
			size_t size = 0,
			size_t offset = 0) = 0;

		// Index buffers

		IWRENDERER_API
		virtual IIndexBuffer* CreateIndexBuffer(
			const void* data,
			size_t count,
			BufferIOType io = STATIC) = 0;

		IWRENDERER_API
		virtual void SetIndexBuffer(
			IIndexBuffer* indexBuffer) = 0;

		// Vertex buffers

		IWRENDERER_API
		virtual IVertexBuffer* CreateVertexBuffer(
			const void* data,
			size_t size,
			BufferIOType io = STATIC) = 0;

		IWRENDERER_API
		virtual void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) = 0;

		// Uniform buffers

		IWRENDERER_API
		virtual IUniformBuffer* CreateUniformBuffer(
			const void* data,
			size_t size,
			BufferIOType io = DYNAMIC) = 0;

		IWRENDERER_API
		virtual void SetUniformBuffer(
			IUniformBuffer* uniformBuffer) = 0;

		// Vertex arrays

		IWRENDERER_API
		virtual IVertexArray* CreateVertexArray() = 0;

		IWRENDERER_API
		virtual void DestroyVertexArray(
			IVertexArray* vertexArray) = 0;

		IWRENDERER_API
		virtual void SetVertexArray(
			IVertexArray* vertexArray) = 0;

		IWRENDERER_API
		virtual void AddBufferToVertexArray(
			IVertexArray* vertexArray,
			IVertexBuffer* buffer,
			const VertexBufferLayout& layout,
			int index = -1) = 0; // if index is negitive the next slot gets used

		IWRENDERER_API
		virtual void UpdateVertexArrayData(
			IVertexArray* vertexArray,
			size_t bufferIndex,
			const void* data,
			size_t size,
			size_t offset = 0) = 0;

		// Vertex shader

		IWRENDERER_API
		virtual IVertexShader* CreateVertexShader(
			const char* source) = 0;

		IWRENDERER_API
		virtual void DestroyVertexArray(
			IVertexShader* vertexShader) = 0;

		// Fragment shader

		IWRENDERER_API
		virtual IFragmentShader* CreateFragmentShader(
			const char* source) = 0;

		IWRENDERER_API
		virtual void DestroyFragmentShader(
			IFragmentShader* fragmentShader) = 0;

		// Geometry shader

		IWRENDERER_API
		virtual IGeometryShader* CreateGeometryShader(
			const char* source) = 0;

		IWRENDERER_API
		virtual void DestroyGeometryShader(
			IGeometryShader* geometryShader) = 0;

		// Compute shader

		IWRENDERER_API
		virtual IComputeShader* CreateComputeShader(
			const char* source) = 0;

		IWRENDERER_API
		virtual void DestroyComputeShader(
			IComputeShader* computeShader) = 0;

		// Shader pipeline

		IWRENDERER_API
		virtual IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader = nullptr) = 0;

		IWRENDERER_API
		virtual void DestroyPipeline(
			IPipeline* pipeline) = 0;

		IWRENDERER_API
		virtual void SetPipeline(
			IPipeline* pipeline) = 0;

		// Compute shader pipeline // prob doesnt do anyhting yet

		IWRENDERER_API
		virtual IComputePipeline* CreateComputePipeline(
			IComputeShader* computeShader) = 0;

		IWRENDERER_API
		virtual void DestroyComputePipeline(
			IComputePipeline* computePipeline) = 0;

		IWRENDERER_API
		virtual void SetComputePipeline(
			IComputePipeline* computePipeline) = 0;

		// Textures

		IWRENDERER_API
		virtual ITexture* CreateTexture(
			int width,
			int height,
			TextureType type,
			TextureFormat format,
			TextureFormatType formatType,
			TextureWrap wrap,
			const void* data = nullptr) = 0;

		IWRENDERER_API
		virtual ITexture* CreateSubTexture(
			ITexture* texture,
			int xOffset,
			int yOffset,
			int width,
			int height,
			int mipmap = 0) = 0;

		IWRENDERER_API
		virtual void DestroyTexture(
			ITexture* texture) = 0;

		IWRENDERER_API
		virtual void SetTexture(
			ITexture* texture) = 0;

		// Frame buffers

		IWRENDERER_API
		virtual IFrameBuffer* CreateFrameBuffer(
			bool noColor) = 0;

		IWRENDERER_API
		virtual void DestroyFrameBuffer(
			IFrameBuffer* frameBuffer) = 0;

		IWRENDERER_API
		virtual void SetFrameBuffer(
			IFrameBuffer* frameBuffer) = 0;

		IWRENDERER_API
		virtual void AttachTextureToFrameBuffer(
			IFrameBuffer* frameBuffer,
			ITexture* texture) = 0;

		IWRENDERER_API
		static IDevice* Create();
	};
}

	using namespace RenderAPI;
}
