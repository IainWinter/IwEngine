#include "iw/graphics/QueuedDevice.h"

// < Operation > 


namespace IwGraphics {
	struct CIB {
		PIB         Buffer;
		size_t      Size;
		const void* Data;
	};

	struct CVB {
		PVB         Buffer;
		size_t      Size;
		const void* Data;
	};

	struct CVA {
		PVA    Array;
		PVB*   Buffers;
		VBL*   Layouts;
		size_t Count;
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

					//args->Array.initialize(Device.CreateVertexArray(args->Count, buffers, args->Layouts));
					args->Array.release();
					for (size_t i = 0; i < args->Count; i++) {
						args->Layouts[i].Clear();
					}

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
		size_t size, 
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
		size_t size, 
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
		size_t numBuffers, 
		PVB* vertexBuffers, 
		VBL* vertexLayouts)
	{
		PVA pva;

		//Copy from stack to scratch
		PVB* buffers = m_scratch.alloc<PVB>(numBuffers);
		VBL* layouts = m_scratch.alloc<VBL>(numBuffers);
		for (size_t i = 0; i < numBuffers; i++) {
			buffers[i] = vertexBuffers[i];
			layouts[i] = vertexLayouts[i]; //Vector gets allocated on the scratch but puts an array on the heap
		}

		CVA* cva = m_scratch.alloc<CVA>();
		cva->Array   = pva;
		cva->Buffers = buffers;
		cva->Layouts = layouts;
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
