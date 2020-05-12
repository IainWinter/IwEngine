#include "iw/events/seq/event_seq.h"
#include "iw/log/logger.h"

void iw::events::event_seq::add(event_seq_item* item)
{
	m_items.push_back(item);
}

void iw::events::event_seq::remove(event_seq_item* item)
{
	m_items.erase(std::find(m_items.begin(), m_items.end(), item));
}

void iw::events::event_seq::update()
{
	if (!m_running) {
		return;
	}
	if (m_current >= m_items.size()) {
		LOG_WARNING << "Tried to update sequence that has finished!";
		return;
	}
	if (m_items[m_current]->update()) {
			m_current++;
	}
}

void iw::events::event_seq::start()
{
	m_running = true;
}

void iw::events::event_seq::stop()
{
	m_running = false;
}
