#include "iw/graphics/RenderQueue.h"

//Should probly be just a part in a renderer

// RenderAPI - Immediate stuff
/// RenderQueue - Async stuff
//

namespace iw {
namespace Graphics {
	RenderQueue::RenderQueue(
		IDevice& device)
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
		QueuedDevice.Execute();

		while (!m_queue.empty()) {
			RenderOperation& op = m_queue.front();

			m_queue.erase(m_queue.begin());
		}

		m_scratch.reset();
	}
}
}
