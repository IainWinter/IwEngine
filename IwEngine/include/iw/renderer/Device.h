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

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API IDevice {
	public:
		virtual void DrawElements(
			MeshTopology topology,
			unsigned count,
			ptrdiff_t offset) = 0;

		virtual void Clear() = 0;

		// should be in frame buffer?
		virtual void SetViewport(
			int x,
			int y) = 0;

		// Index buffers

		virtual IIndexBuffer* CreateIndexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyIndexBuffer(
			IIndexBuffer* indexBuffer) = 0;

		virtual void SetIndexBuffer(
			IIndexBuffer* indexBuffer) = 0;

		// Vertex buffers

		virtual IVertexBuffer* CreateVertexBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyVertexBuffer(
			IVertexBuffer* vertexBuffer) = 0;

		virtual void SetVertexBuffer(
			IVertexBuffer* vertexBuffer) = 0;

		virtual void UpdateVertexBufferData(
			IVertexBuffer* vertexBuffer,
			const void* data,
			size_t size = 0) = 0;

		// Uniform buffers

		virtual IUniformBuffer* CreateUniformBuffer(
			size_t size,
			const void* data = nullptr) = 0;

		virtual void DestroyUniformBuffer(
			IUniformBuffer* uniformBuffer) = 0;

		virtual void SetUniformBuffer(
			IUniformBuffer* uniformBuffer) = 0;

		virtual void UpdateUniformBufferData(
			IUniformBuffer* uniformBuffer,
			const void* data) = 0;

		// Vertex arrays

		virtual IVertexArray* CreateVertexArray() = 0;

		virtual void DestroyVertexArray(
			IVertexArray* vertexArray) = 0;

		virtual void SetVertexArray(
			IVertexArray* vertexArray) = 0;

		virtual void AddBufferToVertexArray(
			IVertexArray* vertexArray,
			IVertexBuffer* buffer,
			const VertexBufferLayout& layout) = 0;

		virtual void UpdateVertexArrayData(
			IVertexArray* vertexArray,
			size_t bufferIndex,
			const void* data,
			size_t size = 0) = 0;

		// Vertex shader

		virtual IVertexShader* CreateVertexShader(
			const char* source) = 0;

		virtual void DestroyVertexArray(
			IVertexShader* vertexShader) = 0;

		// Fragment shader

		virtual IFragmentShader* CreateFragmentShader(
			const char* source) = 0;

		virtual void DestroyFragmentShader(
			IFragmentShader* fragmentShader) = 0;

		// Geometry shader

		virtual IGeometryShader* CreateGeometryShader(
			const char* source) = 0;

		virtual void DestroyGeometryShader(
			IGeometryShader* geometryShader) = 0;

		// Compute shader

		virtual IComputeShader* CreateComputeShader(
			const char* source) = 0;

		virtual void DestroyComputeShader(
			IComputeShader* computeShader) = 0;

		// Shader pipeline

		virtual IPipeline* CreatePipeline(
			IVertexShader* vertexShader,
			IFragmentShader* fragmentShader,
			IGeometryShader* geometryShader = nullptr) = 0;

		virtual void DestroyPipeline(
			IPipeline* pipeline) = 0;

		virtual void SetPipeline(
			IPipeline* pipeline) = 0;

		// Compute shader pipeline // prob doesnt do anyhting yet

		virtual IComputePipeline* CreateComputePipeline(
			IComputeShader* computeShader) = 0;

		virtual void DestroyComputePipeline(
			IComputePipeline* computePipeline) = 0;

		virtual void SetComputePipeline(
			IComputePipeline* computePipeline) = 0;

		// Textures

		virtual ITexture* CreateTexture(
			int width,
			int height,
			TextureFormat format,
			TextureFormatType type,
			const void* data = nullptr) = 0;

		virtual void DestroyTexture(
			ITexture* texture) = 0;

		virtual void SetTexture(
			ITexture* texture) = 0;

		// Frame buffers

		virtual IFrameBuffer* CreateFrameBuffer() = 0;

		virtual void DestroyFrameBuffer(
			IFrameBuffer* frameBuffer) = 0;

		virtual void SetFrameBuffer(
			IFrameBuffer* frameBuffer) = 0;

		virtual void AttachTextureToFrameBuffer(
			IFrameBuffer* frameBuffer,
			ITexture* texture) = 0;

		static IDevice* Create();
	};
}
}
