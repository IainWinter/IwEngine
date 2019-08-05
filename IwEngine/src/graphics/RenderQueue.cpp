#include "iw/graphics/RenderQueue.h"

//Should probly be just a part in a renderer

// Renderer - Immediate stuff
/// RenderQueue - Async stuff
//

namespace IwGraphics {
	RenderQueue::RenderQueue(
		IwRenderer::IDevice& device)
		: Device(device)
		, m_scratch(1024 * 500)
	{}

	void RenderQueue::Push(
		OpCode&& op, 
		void* data)
	{
		m_queue.push_back({ op, data });
	}

	void RenderQueue::Execute() {
		while (!m_queue.empty()) {
			RenderOp& op = m_queue.front();
			switch (op.Code) {
				case CREATE_VERTEX_BUFFER: {
					CVBA* args = (CVBA*)op.Args;
					args->Buffer.initialize(Device.CreateVertexBuffer(args->Size, args->Data));
					break;
				}
				case CREATE_INDEX_BUFFER: {
					CIBA* args = (CIBA*)op.Args;
					args->Buffer.initialize(Device.CreateIndexBuffer(args->Count, args->Data));
					break;
				}
				case CREATE_VERTEX_ARRAY: {
					CVAA* args = (CVAA*)op.Args;
					IwRenderer::IVertexBuffer** buffers = m_scratch.alloc<IwRenderer::IVertexBuffer*>(args->Count);
					for (size_t i = 0; i < args->Count; i++) {
						buffers[i] = args->Buffers[i].value();
					}

					args->Array.initialize(Device.CreateVertexArray(args->Count, buffers, args->Layouts));

					break;
				}
			}

			m_queue.erase(m_queue.begin());
		}

		m_scratch.reset();

		LOG_INFO << m_scratch.peak();
	}

	iwu::potential<IwRenderer::IVertexBuffer*> RenderQueue::CreateVertexBuffer(
		std::size_t size,
		void* data)
	{
		iwu::potential<IwRenderer::IVertexBuffer*> buffer;
		CVBA* args = m_scratch.alloc<CVBA>();
		args->Buffer = buffer;
		args->Size   = size;
		args->Data   = data;

		Push(CREATE_VERTEX_BUFFER, args);

		return buffer;
	}

	iwu::potential<IwRenderer::IIndexBuffer*> RenderQueue::CreateIndexBuffer(
		std::size_t count,
		void* data)
	{
		iwu::potential<IwRenderer::IIndexBuffer*> buffer;
		CIBA* args = m_scratch.alloc<CIBA>();
		args->Buffer = buffer;
		args->Count  = count;
		args->Data   = data;

		Push(CREATE_INDEX_BUFFER, args);

		return buffer;
	}

	iwu::potential<IwRenderer::IVertexArray*> RenderQueue::CreateVertexArray(
		std::size_t count,
		iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
		IwRenderer::VertexBufferLayout* layouts)
	{
		iwu::potential<IwRenderer::IVertexArray*> array;
		CVAA* args = m_scratch.alloc<CVAA>();
		args->Array   = array;
		args->Count   = count;
		args->Buffers = buffers;
		args->Layouts = layouts;

		Push(CREATE_VERTEX_ARRAY, args);

		return array;
	}

	Mesh RenderQueue::CreateMesh(
		std::size_t vertexCount,
		Vertex* vertices,
		unsigned int indexCount,
		unsigned int* indices)
	{
		auto layouts = m_scratch.alloc<IwRenderer::VertexBufferLayout>();
		layouts[0].Push<float>(3);
		layouts[0].Push<float>(3);

		auto pbuffs = m_scratch.alloc<iwu::potential<IwRenderer::IVertexBuffer*>>();
		pbuffs[0] = CreateVertexBuffer(vertexCount * sizeof(Vertex), vertices);

		auto pib = CreateIndexBuffer(indexCount, indices);
		auto pva = CreateVertexArray(1, pbuffs, layouts);

		return Mesh { pva, pib, indexCount };
	}
}
