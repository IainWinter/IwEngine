#include "QueuedDevice.h"

namespace IwGraphics {
	struct CIB {
		iwu::potential<IwRenderer::IIndexBuffer*> Buffer;
		std::size_t Size;
		const void* Data;
	};

	struct CVB {
		iwu::potential<IwRenderer::IVertexBuffer*> Buffer;
		std::size_t Size;
		const void* Data;
	};

	struct DIB {
		iwu::potential<IwRenderer::IIndexBuffer*> Buffer;
	};

	void QueuedDevice::Push(
		Operation&& operation, 
		void* args)
	{
		m_queue.emplace(std::forward<Operation>(operation), args);
	}

	QueuedDevice::QueuedDevice(
		IwRenderer::IDevice& device) 
		: m_scratch(1024 * 10)
		, Device(device)
	{}

	void QueuedDevice::Execute() {
		while (!m_queue.empty()) {
			DeviceOperation& op = m_queue.front();

			switch (op.Operation) {
				case CREATE_INDEX_BUFFER: {
					CIB* args = (CIB*)op.Args;
					args->Buffer.initialize(Device.CreateIndexBuffer(args->Size, args->Data));
					break;
				}
				case DESTROY_INDEX_BUFFER: {
					DIB* args = (DIB*)op.Args;
					Device.DestroyIndexBuffer(args->Buffer.value()); //could be uninitalized
				}
			}

			m_queue.pop();
		}

		m_scratch.reset();
	}

	iwu::potential<IwRenderer::IIndexBuffer*> QueuedDevice::CreateIndexBuffer(
		std::size_t size, 
		const void* data)
	{
		iwu::potential<IwRenderer::IIndexBuffer*> pb;

		CIB* cib = m_scratch.alloc<CIB>();
		cib->Buffer = pb;
		cib->Size   = size;
		cib->Data   = data;

		Push(CREATE_INDEX_BUFFER, cib);

		return pb;
	}

	void QueuedDevice::DestroyIndexBuffer(
		iwu::potential<IwRenderer::IIndexBuffer*> indexBuffer)
	{
		DIB* dib = m_scratch.alloc<DIB>();
		dib->Buffer = indexBuffer;

		Push(DESTROY_INDEX_BUFFER, dib);
	}

	iwu::potential<IwRenderer::IVertexBuffer*> QueuedDevice::CreateVertexBuffer(
		std::size_t size, 
		const void* data)
	{
		iwu::potential<IwRenderer::IVertexBuffer*> pb;

		CVB* cib = m_scratch.alloc<CVB>();
		cib->Buffer = pb;
		cib->Size = size;
		cib->Data = data;

		Push(CREATE_INDEX_BUFFER, cib);

		return pb;
	}

	void QueuedDevice::DestroyVertexBuffer(
		iwu::potential<IwRenderer::IVertexBuffer*> vertexBuffer)
	{

	}

	iwu::potential<IwRenderer::IVertexArray*> QueuedDevice::CreateVertexArray(
		std::size_t numBuffers, 
		iwu::potential<IwRenderer::IVertexBuffer*>* vertexBuffers, 
		IwRenderer::VertexBufferLayout* vertexLayouts)
	{

	}

	iwu::potential<IwRenderer::IVertexShader*> QueuedDevice::CreateVertexShader(
		const char* source)
	{
		return iwu::potential<IwRenderer::IVertexShader*>();
	}

	void QueuedDevice::DestroyVertexShader(
		iwu::potential<IwRenderer::IVertexShader*> vertexShader)
	{

	}

	iwu::potential<IwRenderer::IFragmentShader*> QueuedDevice::CreateFragmentShader(
		const char* source)
	{
		return iwu::potential<IwRenderer::IFragmentShader*>();
	}

	void QueuedDevice::DestroyFragmentShader(
		iwu::potential<IwRenderer::IFragmentShader*> fragmentShader)
	{

	}

	iwu::potential<IwRenderer::IGeometryShader*> QueuedDevice::CreateGeometryShader(
		const char* source)
	{
		return iwu::potential<IwRenderer::IGeometryShader*>();
	}

	void QueuedDevice::DestroyGeometryShader(
		iwu::potential<IwRenderer::IGeometryShader*> geometryShader)
	{

	}

	iwu::potential<IwRenderer::IComputeShader*> QueuedDevice::CreateComputeShader(
		const char* source)
	{
		return iwu::potential<IwRenderer::IComputeShader*>();
	}

	void QueuedDevice::DestroyComputeShader(
		iwu::potential<IwRenderer::IComputeShader*> computeShader)
	{

	}

	iwu::potential<IwRenderer::IPipeline*> QueuedDevice::CreatePipeline(
		iwu::potential<IwRenderer::IVertexShader*> vertexShader, 
		iwu::potential<IwRenderer::IFragmentShader*> fragmentShader, 
		iwu::potential<IwRenderer::IGeometryShader*> geometryShader)
	{
		return iwu::potential<IwRenderer::IPipeline*>();
	}

	void QueuedDevice::DestroyPipeline(
		iwu::potential<IwRenderer::IPipeline*> pipeline)
	{

	}

	iwu::potential<IwRenderer::IComputePipeline*> QueuedDevice::CreateComputePipeline(
		iwu::potential<IwRenderer::IComputeShader*> computeShader)
	{
		return iwu::potential<IwRenderer::IComputePipeline*>();
	}

	void QueuedDevice::DestroyComputePipeline(
		iwu::potential<IwRenderer::IComputePipeline*> computePipeline)
	{

	}

	void QueuedDevice::DestroyVertexArray(
		iwu::potential<IwRenderer::IVertexArray*> vertexArray)
	{

	}
}

//iwu::potential<IwRenderer::IVertexBuffer*> CreateVertexBuffer(
//	std::size_t size,
//	void* data);
//
//iwu::potential<IwRenderer::IIndexBuffer*> CreateIndexBuffer(
//	std::size_t count,
//	void* data);
//
//// TODO: Find out how this would work with multiple potential buffers
//iwu::potential<IwRenderer::IVertexArray*> CreateVertexArray(
//	std::size_t count,
//	iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
//	IwRenderer::VertexBufferLayout* layouts);
//
//iwu::potential<IwRenderer::IVertexArray*> CreateVertexArray(
//	std::size_t count,
//	IwRenderer::IVertexBuffer** buffers,
//	IwRenderer::VertexBufferLayout* layouts);
//
//Mesh CreateMesh(
//	std::size_t vertexCount,
//	Vertex* vertices,
//	unsigned int indexCount,
//	unsigned int* indices);
//
//iwu::potential<IwRenderer::IVertexBuffer*> RenderQueue::CreateVertexBuffer(
//	std::size_t size,
//	void* data)
//{
//	iwu::potential<IwRenderer::IVertexBuffer*> buffer;
//	CVBA* args = m_scratch.alloc<CVBA>();
//	args->Buffer = buffer;
//	args->Size = size;
//	args->Data = data;
//
//	Push(CREATE_VERTEX_BUFFER, args);
//
//	return buffer;
//}
//
//iwu::potential<IwRenderer::IIndexBuffer*> RenderQueue::CreateIndexBuffer(
//	std::size_t count,
//	void* data)
//{
//	iwu::potential<IwRenderer::IIndexBuffer*> buffer;
//	CIBA* args = m_scratch.alloc<CIBA>();
//	args->Buffer = buffer;
//	args->Count = count;
//	args->Data = data;
//
//	Push(CREATE_INDEX_BUFFER, args);
//
//	return buffer;
//}
//
//iwu::potential<IwRenderer::IVertexArray*> RenderQueue::CreateVertexArray(
//	std::size_t count,
//	iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
//	IwRenderer::VertexBufferLayout* layouts)
//{
//	iwu::potential<IwRenderer::IVertexArray*> array;
//	CVAA* args = m_scratch.alloc<CVAA>();
//	args->Array = array;
//	args->Count = count;
//	args->Buffers = buffers;
//	args->Layouts = layouts;
//
//	Push(CREATE_VERTEX_ARRAY, args);
//
//	return array;
//}
//
////TODO: test if this works
//iwu::potential<IwRenderer::IVertexArray*> RenderQueue::CreateVertexArray(
//	std::size_t count,
//	IwRenderer::IVertexBuffer** buffers,
//	IwRenderer::VertexBufferLayout* layouts)
//{
//	auto vbuffs = m_scratch.alloc<iwu::potential<IwRenderer::IVertexBuffer*>>();
//	for (size_t i = 0; i < count; i++) {
//		vbuffs[0].initialize(buffers[i]);
//	}
//
//	return CreateVertexArray(count, vbuffs, layouts);
//}
//
//Mesh RenderQueue::CreateMesh(
//	std::size_t vertexCount,
//	Vertex* vertices,
//	unsigned int indexCount,
//	unsigned int* indices)
//{
//	auto layouts = m_scratch.alloc<IwRenderer::VertexBufferLayout>();
//	layouts[0].Push<float>(3);
//	layouts[0].Push<float>(3);
//
//	auto pbuffs = m_scratch.alloc<iwu::potential<IwRenderer::IVertexBuffer*>>();
//	pbuffs[0] = CreateVertexBuffer(vertexCount * sizeof(Vertex), vertices);
//
//	auto pib = CreateIndexBuffer(indexCount, indices);
//	auto pva = CreateVertexArray(1, pbuffs, layouts);
//
//	return Mesh{ pva, pib, indexCount };
//}
