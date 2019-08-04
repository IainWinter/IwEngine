#include "iw/graphics/RenderQueue.h"

//Should probly be just a part in a renderer

// Renderer - Immediate stuff
/// RenderQueue - Async stuff
//

namespace IwGraphics {
	RenderQueue::CVBA::CVBA(
		iwu::potential<IwRenderer::IVertexBuffer*> buffer, 
		std::size_t size, 
		const void* data)
		: Buffer(buffer)
		, Size(size)
		, Data(data)
	{}

	RenderQueue::CIBA::CIBA(
		iwu::potential<IwRenderer::IIndexBuffer*> buffer, 
		std::size_t count, 
		const void* data)
		: Buffer(buffer)
		, Count(count)
		, Data(data)
	{}

	RenderQueue::CVAA::CVAA(
		iwu::potential<IwRenderer::IVertexArray*> array,
		std::size_t count, 
		iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
		IwRenderer::VertexBufferLayout** layouts)
		: Array(array)
		, Buffers(buffers)
		, Layouts(layouts)
		, Count(count)
	{}

	RenderQueue::RenderQueue(
		IwRenderer::IDevice& device)
		: m_device(device)
	{}

	void RenderQueue::Push(
		OpCode&& op, 
		Args* data)
	{
		m_queue.push_back({ op, data });
	}

	void RenderQueue::Execute() {
		while (!m_queue.empty()) {
			RenderOp& op = m_queue.front();
			switch (op.Code) {
				case CREATE_VERTEX_BUFFER: {
					CVBA& args = *(CVBA*)op.Args;
					args.Buffer.initialize(m_device.CreateVertexBuffer(args.Size, args.Data));
					break;
				}
				case CREATE_INDEX_BUFFER: {
					CIBA& args = *(CIBA*)op.Args;
					args.Buffer.initialize(m_device.CreateIndexBuffer(args.Count, args.Data));
					break;
				}
				case CREATE_VERTEX_ARRAY: {
					CVAA& args = *(CVAA*)op.Args;

					IwRenderer::IVertexBuffer** buffers = new IwRenderer::IVertexBuffer*[args.Count];
					for (size_t i = 0; i < args.Count; i++) {
						buffers[i] = args.Buffers[i].value();
					}

					args.Array.initialize(m_device.CreateVertexArray(args.Count, buffers, args.Layouts));

					delete[] buffers;

					break;
				}
			}

			delete op.Args;
			m_queue.erase(m_queue.begin());
		}
	}

	iwu::potential<IwRenderer::IVertexBuffer*> RenderQueue::CreateVertexBuffer(
		std::size_t size,
		void* data)
	{
		iwu::potential<IwRenderer::IVertexBuffer*> p;

		Push(CREATE_VERTEX_BUFFER, new CVBA {
			p, size, data
		});

		return p;
	}

	iwu::potential<IwRenderer::IIndexBuffer*> RenderQueue::CreateIndexBuffer(
		std::size_t count,
		void* data)
	{
		iwu::potential<IwRenderer::IIndexBuffer*> p;

		Push(CREATE_INDEX_BUFFER, new CIBA {
			p, count, data
		});

		return p;
	}

	iwu::potential<IwRenderer::IVertexArray*> RenderQueue::CreateVertexArray(
		std::size_t count,
		iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
		IwRenderer::VertexBufferLayout** layouts)
	{
		iwu::potential<IwRenderer::IVertexArray*> p;

		Push(CREATE_VERTEX_ARRAY, new CVAA {
			p, count, buffers, layouts
		});

		return p;
	}

	Mesh RenderQueue::CreateMesh(
		std::size_t vertexCount,
		Vertex* vertices,
		unsigned int indexCount,
		unsigned int* indices)
	{
		auto layouts = new IwRenderer::VertexBufferLayout * [1];
		layouts[0] = new IwRenderer::VertexBufferLayout();
		layouts[0]->Push<float>(3);
		layouts[0]->Push<float>(3);

		auto pbuffs = new iwu::potential<IwRenderer::IVertexBuffer*>[1];
		pbuffs[0] = CreateVertexBuffer(vertexCount * sizeof(Vertex), vertices);

		auto pib = CreateIndexBuffer(indexCount, indices);
		auto pva = CreateVertexArray(1, pbuffs, layouts);

		return Mesh{ pva, pib, indexCount };
	}
}
