#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename _t>
struct tsque
{
	std::deque<_t> m_queue;
	std::mutex m_mutex;
	std::condition_variable m_condition;

	void clear()
	{
		std::scoped_lock lock(m_mutex);
		m_queue.clear();
	}

	void push_back(const _t& t)
	{
		std::scoped_lock lock(m_mutex);
		m_queue.push_back(t);
		m_condition.notify_one();
	}

	void push_front(const _t& t)
	{
		std::scoped_lock lock(m_mutex);
		m_queue.push_front(t);
		m_condition.notify_one();
	}

	_t pop_back()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [&] { return !m_queue.empty(); });

		_t out = std::move(m_queue.back());
		m_queue.pop_back();
		return out;
	}

	_t pop_front()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_condition.wait(lock, [&] { return !m_queue.empty(); });

		_t out = std::move(m_queue.front());
		m_queue.pop_front();
		return out;
	}

	// 99% sure these should return copies for mt

	_t& back()
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.back();
	}

	_t& front()
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.front();
	}

	_t& at(size_t index)
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.at(index);
	}

	void erase(size_t index)
	{
		std::scoped_lock lock(m_mutex);
		m_queue.erase(m_queue.begin() + index);
	}

	// cant be const because of wait condition?

	size_t size()
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.size();
	}

	bool empty()
	{
		std::scoped_lock lock(m_mutex);
		return m_queue.empty();
	}
};