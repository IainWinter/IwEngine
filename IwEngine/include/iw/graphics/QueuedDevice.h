#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/util/async/potential.h"
#include <queue>

namespace IW {
inline namespace Graphics {
	using PIB = iwu::potential<IW::IIndexBuffer*>;
	using PVB = iwu::potential<IW::IVertexBuffer*>;
	using PVA = iwu::potential<IW::IVertexArray*>;
	using VBL = IW::VertexBufferLayout;

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
		IW::IDevice& Device;

	private:
		void Push(
			Operation operation,
			void* args);
	public:
		QueuedDevice(
			IW::IDevice& device);

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
		iwu::potential<IW::IVertexShader*> CreateVertexShader(
			const char* source);

		void DestroyVertexShader(
			iwu::potential<IW::IVertexShader*> vertexShader);

		//Fragment shader
		iwu::potential<IW::IFragmentShader*> CreateFragmentShader(
			const char* source);

		void DestroyFragmentShader(
			iwu::potential<IW::IFragmentShader*> fragmentShader);

		//Geometry shader
		iwu::potential<IW::IGeometryShader*> CreateGeometryShader(
			const char* source);

		void DestroyGeometryShader(
			iwu::potential<IW::IGeometryShader*> geometryShader);

		//Compute shader
		iwu::potential<IW::IComputeShader*> CreateComputeShader(
			const char* source);

		void DestroyComputeShader(
			iwu::potential<IW::IComputeShader*> computeShader);

		//Shader pipeline
		iwu::potential<IW::IPipeline*> CreatePipeline(
			iwu::potential<IW::IVertexShader*>   vertexShader,
			iwu::potential<IW::IFragmentShader*> fragmentShader,
			iwu::potential<IW::IGeometryShader*> geometryShader = nullptr);

		void DestroyPipeline(
			iwu::potential<IW::IPipeline*> pipeline);

		//Compute shader pipeline
		iwu::potential<IW::IComputePipeline*> CreateComputePipeline(
			iwu::potential<IW::IComputeShader*> computeShader);

		void DestroyComputePipeline(
			iwu::potential<IW::IComputePipeline*> computePipeline);
	};
}
}
