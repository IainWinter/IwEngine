#include "iw/events/seq/event_seq.h"
#include "iw/log/logger.h"

namespace iw {
namespace events {
	event_seq::event_seq()
		: m_current(0)
		, m_running(false)
	{}

	event_seq::~event_seq() {
		for (event_task* task : m_tasks) {
			delete task;
		}
	}

	void event_seq::add(
		event_task* task) // peep argument(s) on new line(s)
	{
		m_tasks.push_back(task);
	}

	void event_seq::remove(
		event_task* task)
	{
		m_tasks.erase(std::find(m_tasks.begin(), m_tasks.end(), task));
	}

	void event_seq::reset() {
		m_current = 0;
		for (event_task* task : m_tasks) {
			task->reset();
		}
	}

	void event_seq::update() {
		if (!m_running) {
			return;
		}

		if (m_current >= m_tasks.size()) {
			stop();

			LOG_WARNING << "Tried to update event sequence that has already finished!";
			return;
		}

		if (m_tasks[m_current]->update()) {
			m_current++;

			if (m_current == m_tasks.size()) {
				stop();
			}
		}
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

