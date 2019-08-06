#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/util/async/potential.h"
#include <queue>

namespace IwGraphics {
	using PIB = iwu::potential<IwRenderer::IIndexBuffer*>;
	using PVB = iwu::potential<IwRenderer::IVertexBuffer*>;
	using PVA = iwu::potential<IwRenderer::IVertexArray*>;
	using VBL = IwRenderer::VertexBufferLayout;

	class IWGRAPHICS_API QueuedDevice {
	private:
		enum Operation {
			CREATE_INDEX_BUFFER,
			CREATE_VERTEX_BUFFER,
			CREATE_VERTEX_ARRAY,
			DESTROY_INDEX_BUFFER,
			DESTROY_VERTEX_BUFFER,
			DESTROY_VERTEX_ARRAY
		};

		struct DeviceOperation {
			Operation Operation;
			void*     Args;
		};

		std::queue<DeviceOperation> m_queue;
		iwu::linear_allocator       m_scratch;
	public:
		IwRenderer::IDevice& Device;

	private:
		void Push(
			Operation operation,
			void* args);
	public:
		QueuedDevice(
			IwRenderer::IDevice& device);

		void Execute();

		//Index buffers
		PIB CreateIndexBuffer(
			std::size_t size,
			const void* data = nullptr);

		void DestroyIndexBuffer(
			PIB indexBuffer);

		//Vertex buffers
		PVB CreateVertexBuffer(
			std::size_t size,
			const void* data = nullptr);

		void DestroyVertexBuffer(
			PVB vertexBuffer);

		//Vertex arrays
		PVA CreateVertexArray(
			std::size_t numBuffers,
			PVB* vertexBuffers,
			VBL* vertexLayouts);

		void DestroyVertexArray(
			PVA vertexArray);

		//Vertex shader
		iwu::potential<IwRenderer::IVertexShader*> CreateVertexShader(
			const char* source);

		void DestroyVertexShader(
			iwu::potential<IwRenderer::IVertexShader*> vertexShader);

		//Fragment shader
		iwu::potential<IwRenderer::IFragmentShader*> CreateFragmentShader(
			const char* source);

		void DestroyFragmentShader(
			iwu::potential<IwRenderer::IFragmentShader*> fragmentShader);

		//Geometry shader
		iwu::potential<IwRenderer::IGeometryShader*> CreateGeometryShader(
			const char* source);

		void DestroyGeometryShader(
			iwu::potential<IwRenderer::IGeometryShader*> geometryShader);

		//Compute shader
		iwu::potential<IwRenderer::IComputeShader*> CreateComputeShader(
			const char* source);

		void DestroyComputeShader(
			iwu::potential<IwRenderer::IComputeShader*> computeShader);

		//Shader pipeline
		iwu::potential<IwRenderer::IPipeline*> CreatePipeline(
			iwu::potential<IwRenderer::IVertexShader*>   vertexShader,
			iwu::potential<IwRenderer::IFragmentShader*> fragmentShader,
			iwu::potential<IwRenderer::IGeometryShader*> geometryShader = nullptr);

		void DestroyPipeline(
			iwu::potential<IwRenderer::IPipeline*> pipeline);

		//Compute shader pipeline
		iwu::potential<IwRenderer::IComputePipeline*> CreateComputePipeline(
			iwu::potential<IwRenderer::IComputeShader*> computeShader);

		void DestroyComputePipeline(
			iwu::potential<IwRenderer::IComputePipeline*> computePipeline);
	};
}
