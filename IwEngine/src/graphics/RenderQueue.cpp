#include "iw/graphics/RenderQueue.h"

//Should probly be just a part in a renderer

// Renderer - Immediate stuff
/// RenderQueue - Async stuff
//

namespace IwGraphics {
	RenderQueue::RenderQueue(
		IwRenderer::IDevice& device)
		: QueuedDevice(device)
		, m_scratch(1024 * 500)
	{}

	void RenderQueue::Push(
		Operation&& op, 
		void* data)
	{
		m_queue.push_back({ op, data });
	}

	void RenderQueue::Execute() {
		while (!m_queue.empty()) {
			RenderOperation& op = m_queue.front();
			switch (op.Operation) {

			}

			m_queue.erase(m_queue.begin());
		}

		m_scratch.reset();

		LOG_INFO << m_scratch.peak();
	}
}
