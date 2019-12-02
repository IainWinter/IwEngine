#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/util/async/potential.h"
#include <queue>

namespace IW {
inline namespace Graphics {
	using PIB = iw::potential<IIndexBuffer*>;
	using PVB = iw::potential<IVertexBuffer*>;
	using PVA = iw::potential<IVertexArray*>;
	using VBL = VertexBufferLayout;

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
		iw::linear_allocator       m_scratch;
	public:
		IDevice& Device;

	private:
		void Push(
			Operation operation,
			void* args);
	public:
		QueuedDevice(
			IDevice& device);

		void Execute();

		//Index buffers
		PIB CreateIndexBuffer(
			size_t size,
			const void* data = nullptr);

		void DestroyIndexBuffer(
			PIB indexBuffer);

		//Vertex buffers
		PVB CreateVertexBuffer(
			size_t size,
			const void* data = nullptr);

		void DestroyVertexBuffer(
			PVB vertexBuffer);

		//Vertex arrays
		PVA CreateVertexArray(
			size_t numBuffers,
			PVB* vertexBuffers,
			VBL* vertexLayouts);

		void DestroyVertexArray(
			PVA vertexArray);

		//Vertex shader
		iw::potential<IVertexShader*> CreateVertexShader(
			const char* source);

		void DestroyVertexShader(
			iw::potential<IVertexShader*> vertexShader);

		//Fragment shader
		iw::potential<IFragmentShader*> CreateFragmentShader(
			const char* source);

		void DestroyFragmentShader(
			iw::potential<IFragmentShader*> fragmentShader);

		//Geometry shader
		iw::potential<IGeometryShader*> CreateGeometryShader(
			const char* source);

		void DestroyGeometryShader(
			iw::potential<IGeometryShader*> geometryShader);

		//Compute shader
		iw::potential<IComputeShader*> CreateComputeShader(
			const char* source);

		void DestroyComputeShader(
			iw::potential<IComputeShader*> computeShader);

		//Shader pipeline
		iw::potential<IPipeline*> CreatePipeline(
			iw::potential<IVertexShader*>   vertexShader,
			iw::potential<IFragmentShader*> fragmentShader,
			iw::potential<IGeometryShader*> geometryShader = nullptr);

		void DestroyPipeline(
			iw::potential<IPipeline*> pipeline);

		//Compute shader pipeline
		iw::potential<IComputePipeline*> CreateComputePipeline(
			iw::potential<IComputeShader*> computeShader);

		void DestroyComputePipeline(
			iw::potential<IComputePipeline*> computePipeline);
	};
}
}
