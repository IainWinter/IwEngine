#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/util/async/potential.h"
#include <queue>

namespace IW {
inline namespace Graphics {
	using PIB = iw::potential<IW::IIndexBuffer*>;
	using PVB = iw::potential<IW::IVertexBuffer*>;
	using PVA = iw::potential<IW::IVertexArray*>;
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
		iw::linear_allocator       m_scratch;
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
		iw::potential<IW::IVertexShader*> CreateVertexShader(
			const char* source);

		void DestroyVertexShader(
			iw::potential<IW::IVertexShader*> vertexShader);

		//Fragment shader
		iw::potential<IW::IFragmentShader*> CreateFragmentShader(
			const char* source);

		void DestroyFragmentShader(
			iw::potential<IW::IFragmentShader*> fragmentShader);

		//Geometry shader
		iw::potential<IW::IGeometryShader*> CreateGeometryShader(
			const char* source);

		void DestroyGeometryShader(
			iw::potential<IW::IGeometryShader*> geometryShader);

		//Compute shader
		iw::potential<IW::IComputeShader*> CreateComputeShader(
			const char* source);

		void DestroyComputeShader(
			iw::potential<IW::IComputeShader*> computeShader);

		//Shader pipeline
		iw::potential<IW::IPipeline*> CreatePipeline(
			iw::potential<IW::IVertexShader*>   vertexShader,
			iw::potential<IW::IFragmentShader*> fragmentShader,
			iw::potential<IW::IGeometryShader*> geometryShader = nullptr);

		void DestroyPipeline(
			iw::potential<IW::IPipeline*> pipeline);

		//Compute shader pipeline
		iw::potential<IW::IComputePipeline*> CreateComputePipeline(
			iw::potential<IW::IComputeShader*> computeShader);

		void DestroyComputePipeline(
			iw::potential<IW::IComputePipeline*> computePipeline);
	};
}
}
