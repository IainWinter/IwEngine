#pragma once

#include "IwGraphics.h"
#include "Mesh.h"
#include "Vertex.h"
#include "iw/log/logger.h"
#include "iw/util/async/potential.h"
#include "iw/renderer/VertexBufferLayout.h"
#include "iw/renderer/VertexBuffer.h"
#include "iw/renderer/VertexArray.h"
#include "iw/renderer/IndexBuffer.h"
#include "iw/renderer/Device.h"
#include <queue>

namespace IwGraphics {
	class IWGRAPHICS_API RenderQueue {
	private:
		enum OpCode {
			CREATE_VERTEX_BUFFER,
			CREATE_INDEX_BUFFER,
			CREATE_VERTEX_ARRAY,
			CREATE_MESH
		};

		// leakky as old man asshoul

		struct Args {
			virtual ~Args() { }
		};

		struct CVBA
			: Args
		{
			iwu::potential<IwRenderer::IVertexBuffer*> Buffer;
			std::size_t Size;
			const void* Data;

			CVBA(
				iwu::potential<IwRenderer::IVertexBuffer*> buffer,
				std::size_t size,
				const void* data);
		};

		struct CIBA
			: Args
		{
			iwu::potential<IwRenderer::IIndexBuffer*> Buffer;
			std::size_t Count;
			const void* Data;

			CIBA(
				iwu::potential<IwRenderer::IIndexBuffer*> buffer,
				std::size_t count,
				const void* data);
		};

		struct CVAA
			: Args
		{
			iwu::potential<IwRenderer::IVertexArray*>   Array;
			iwu::potential<IwRenderer::IVertexBuffer*>* Buffers;
			IwRenderer::VertexBufferLayout** Layouts;
			std::size_t Count;

			CVAA(
				iwu::potential<IwRenderer::IVertexArray*> buffer,
				std::size_t count,
				iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
				IwRenderer::VertexBufferLayout** layouts);
		};

		struct RenderOp {
			OpCode      Code;
			const Args* Args;
			//RenderOp*   Next; //more complex ops probly need this
		};

		std::vector<RenderOp> m_queue;
		IwRenderer::IDevice&  m_device;

	public:
		RenderQueue(
			IwRenderer::IDevice& device);

		void Push(
			OpCode&& op,
			Args* data);

		void Execute();

		inline IwRenderer::IDevice& Device() {
			return m_device;
		}

		iwu::potential<IwRenderer::IVertexBuffer*> CreateVertexBuffer(
			std::size_t size,
			void* data);

		iwu::potential<IwRenderer::IIndexBuffer*> CreateIndexBuffer(
			std::size_t count,
			void* data);

		// TODO: Find out how this would work with multiple potential buffers
		iwu::potential<IwRenderer::IVertexArray*> CreateVertexArray(
			std::size_t count,
			iwu::potential<IwRenderer::IVertexBuffer*>* buffers,
			IwRenderer::VertexBufferLayout** layouts);

		Mesh CreateMesh(
			std::size_t vertexCount,
			Vertex* vertices,
			unsigned int indexCount,
			unsigned int* indices);
	};
}
