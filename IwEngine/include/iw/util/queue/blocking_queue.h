#pragma once

#include "iw/util/iwutil.h"
#include <mutex>
#include <queue>

namespace iwutil {
	/**
	* @brief Super simple blocking queue.
	*
	* Blocking queues are useful when blocking of the current thread
	* is desired while the queue is empty.
	*
	* @tparam _t The type to store in the queue.
	*/
	template<
		typename _t>
	class blocking_queue {
	private:
		std::mutex              m_mutex;
		std::condition_variable m_condition;
		std::queue<_t>          m_queue;

	public:
		/**
		* Pushes an item to the back of the queue.
		*
		* @param item The item to push.
		*/
		void push(
			const _t& item)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_queue.push(item);

			lock.unlock();
			m_condition.notify_one();
		}

		/**
		* Pops an item from the front of the queue.
		*/
		_t pop() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition.wait(lock, [=] { return !m_queue.empty(); });

			_t item = std::move(m_queue.front());
			m_queue.pop();

			return item;
		}
	};
}
