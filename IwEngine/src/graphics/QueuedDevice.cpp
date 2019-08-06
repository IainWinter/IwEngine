#include "iw/graphics/QueuedDevice.h"

namespace IwGraphics {
	using PIB = iwu::potential<IwRenderer::IIndexBuffer*>;
	using PVB = iwu::potential<IwRenderer::IVertexBuffer*>;
	using PVA = iwu::potential<IwRenderer::IVertexArray*>;
	using VBL = IwRenderer::VertexBufferLayout;

	struct CIB {
		PIB         Buffer;
		std::size_t Size;
		const void* Data;
	};

	struct CVB {
		PVB         Buffer;
		std::size_t Size;
		const void* Data;
	};

	struct CVA {
		PVA         Array;
		PVB*        Buffers;
		VBL*        Layouts;
		std::size_t Count;
	};

	struct DIB {
		PIB Buffer;
	};

	struct DVB {
		PVB Buffer;
	};

	struct DVA {
		PVA Array;
	};

	void QueuedDevice::Push(
		Operation operation, 
		void* args)
	{
		m_queue.push({ operation, args });
	}

	QueuedDevice::QueuedDevice(
		IwRenderer::IDevice& device) 
		: m_scratch(1024 * 1024)
		, Device(device)
	{}

	void QueuedDevice::Execute() {
		while (!m_queue.empty()) {
			DeviceOperation& op = m_queue.front();

			switch (op.Operation) {
				case CREATE_INDEX_BUFFER: {
					CIB* args = (CIB*)op.Args;
					args->Buffer.initialize(Device.CreateIndexBuffer(args->Size, args->Data));
					args->Buffer.release();
					break;
				}
				case CREATE_VERTEX_BUFFER: {
					CVB* args = (CVB*)op.Args;
					args->Buffer.initialize(Device.CreateVertexBuffer(args->Size, args->Data));
					args->Buffer.release();
					break;
				}
				case CREATE_VERTEX_ARRAY: {
					CVA* args = (CVA*)op.Args;
					
					IwRenderer::IVertexBuffer** buffers = m_scratch.alloc<IwRenderer::IVertexBuffer*>(args->Count);
					for (size_t i = 0; i < args->Count; i++) {
						buffers[i] = args->Buffers[i].consume();
					}

					args->Array.initialize(Device.CreateVertexArray(args->Count, buffers, args->Layouts));
					args->Array.release();
					break;
				}
				case DESTROY_INDEX_BUFFER: {
					DIB* args = (DIB*)op.Args;
					Device.DestroyIndexBuffer(args->Buffer.consume()); //could be uninitalized
					break;
				}
				case DESTROY_VERTEX_BUFFER: {
					CVB* args = (CVB*)op.Args;
					Device.DestroyVertexBuffer(args->Buffer.consume()); //could be uninitalized
					break;
				}
				case DESTROY_VERTEX_ARRAY: {
					CVA* args = (CVA*)op.Args;
					Device.DestroyVertexArray(args->Array.consume()); //could be uninitalized
					break;
				}
			}

			m_queue.pop();
		}

		m_scratch.reset();
	}

	PIB QueuedDevice::CreateIndexBuffer(
		std::size_t size, 
		const void* data)
	{
		PIB pib;

		CIB* cib = m_scratch.alloc<CIB>();
		cib->Buffer = pib;
		cib->Size   = size;
		cib->Data   = data;

		Push(CREATE_INDEX_BUFFER, cib);

		return pib;
	}

	void QueuedDevice::DestroyIndexBuffer(
		PIB indexBuffer)
	{
		DIB* dib = m_scratch.alloc<DIB>();
		dib->Buffer = indexBuffer;

		Push(DESTROY_INDEX_BUFFER, dib);
	}

	PVB QueuedDevice::CreateVertexBuffer(
		std::size_t size, 
		const void* data)
	{
		PVB pvb;

		CVB* cib = m_scratch.alloc<CVB>();
		cib->Buffer = pvb;
		cib->Size = size;
		cib->Data = data;

		Push(CREATE_VERTEX_BUFFER, cib);

		return pvb;
	}

	void QueuedDevice::DestroyVertexBuffer(
		PVB vertexBuffer)
	{
		DVB* dvb = m_scratch.alloc<DVB>();
		dvb->Buffer = vertexBuffer;

		Push(DESTROY_VERTEX_BUFFER, dvb);
	}

	PVA QueuedDevice::CreateVertexArray(
		std::size_t numBuffers, 
		PVB* vertexBuffers, 
		VBL* vertexLayouts)
	{
		PVA pva;

		//Copy from stack to scratch
		PVB* buffers = m_scratch.alloc<PVB>(numBuffers);
		VBL* layouts = m_scratch.alloc<VBL>(numBuffers);
		for (size_t i = 0; i < numBuffers; i++) {
			buffers[i] = vertexBuffers[i];
			layouts[i] = vertexLayouts[i];
		}

		CVA* cva = m_scratch.alloc<CVA>();
		cva->Array   = pva;
		cva->Buffers = buffers;
		cva->Layouts  = layouts;
		cva->Count   = numBuffers;

		Push(CREATE_VERTEX_ARRAY, cva);

		return pva;
	}

	void QueuedDevice::DestroyVertexArray(
		PVA vertexArray)
	{
		DVA* dva = m_scratch.alloc<DVA>();
		dva->Array = vertexArray;

		Push(DESTROY_VERTEX_ARRAY, dva);
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
