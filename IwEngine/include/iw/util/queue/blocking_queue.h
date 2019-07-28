#pragma once

#include "iw/util/iwutil.h"
#include <mutex>
#include <queue>

namespace iwutil {
	template<
		typename _t>
	class blocking_queue {
	private:
		std::mutex              m_mutex;
		std::condition_variable m_condition;
		std::queue<_t>          m_queue;

	public:
		void push(
			const _t& item)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.push(item);
			m_condition.notify_one();
		}

		_t pop() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition.wait(lock, [=] { return !m_queue.empty(); });
			_t item = std::move(m_queue.back());
			m_queue.pop();
			return item;
		}
	};
}
