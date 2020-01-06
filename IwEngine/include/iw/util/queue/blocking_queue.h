#pragma once

#include "iw/util/iwutil.h"
#include <mutex>
#include <queue>

namespace iw {
namespace util {
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
		std::deque<_t>          m_queue;

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

			m_queue.push_back(item);

			//lock.unlock();
			m_condition.notify_one();
		}

		/**
		* Pops an item from the front of the queue.
		*/
		_t pop() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_condition.wait(lock, [=] { return !m_queue.empty(); });

			_t item = std::move(m_queue.front());
			m_queue.pop_front();

			return item;
		}

		void clear() {
			std::unique_lock<std::mutex> lock(m_mutex);
			m_queue.clear();

			//lock.unlock();
			m_condition.notify_one();
		}

		bool empty() {
			return m_queue.empty(); // not sure if this is thread safe?
		}

		size_t size() {
			return m_queue.size();
		}
	};
}

	using namespace util;
}
