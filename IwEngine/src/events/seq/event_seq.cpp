#include "iw/events/seq/event_seq.h"
#include "iw/log/logger.h"

namespace iw {
namespace events {
	event_seq::event_seq()
		: m_current(0)
		, m_running(false)
	{}

	event_seq::~event_seq() {
		clear();
	}

	event_seq& event_seq::add(
		event_task* task)
	{
		m_tasks.push_back(task);
		return *this;
	}

	event_seq& event_seq::add(
		std::function<bool(void)> func)
	{
		add(new event_func(func));
		return *this;
	}

	event_seq& event_seq::and(
		std::function<bool(void)> func)
	{
		and(new event_func(func));
		return *this;
	}

	event_seq& event_seq::and(
		event_task* task)
	{
		if (m_tasks.size() == 0)
		{
			LOG_WARNING << "Tried to and task to empty sequence! Call add first";
			return *this;
		}

		event_task* node = m_tasks.back();
		while (node->m_child)
		{
			node = node->m_child;
		}

		node->m_child = task;

		return *this;
	}

	void event_seq::remove(
		event_task* task)
	{
		m_tasks.erase(std::find(m_tasks.begin(), m_tasks.end(), task));
	}

	void event_seq::clear() {
		for (event_task* task : m_tasks) {
			delete task;
		}

		m_tasks.clear();
	}

	void event_seq::reset() {
		m_current = 0;
		for (event_task* task : m_tasks) {
			task->reset_tree();
		}
	}

	bool event_seq::update() {
		if (!m_running) {
			return false;
		}

		if (m_current >= m_tasks.size()) {
			stop();

			LOG_WARNING << "Tried to update event sequence that has already finished!";
			return true;
		}

		if (m_tasks[m_current]->update_tree()) {
			m_current++;

			if (m_current == m_tasks.size()) {
				stop();
				return true;
			}
		}

		return false;
	}

	void event_seq::start() {
		m_running = true;
	}

	void event_seq::stop() {
		m_running = false;
	}

	void event_seq::restart() {
		reset();
		start();
	}
}
}

