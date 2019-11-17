#pragma once

#include "iw/events/event.h"
#include <vector>

namespace IW {
inline namespace Engine {
	// T needs to have a 'const char* Name()' function
	// and bool 'On(Event&)' functions for every event
	template<
		typename T>
	class EventStack {
	public:
		using iterator = typename std::vector<T>::iterator;
	private:
		std::vector<T> m_items;

	public:
		void PushBack(
			T&& item)
		{
			m_items.emplace_back(std::forward<T>(item));
		}

		void PushBack(
			const T& item)
		{
			m_items.push_back(item);
		}

		void PushFront(
			T&& item)
		{
			m_items.emplace(begin(), std::forward<T>(item));
		}

		void PushFront(
			const T& item)
		{
			m_items.insert(begin(), item);
		}

		void Pop(
			const char* name)
		{
			auto itr = m_items.begin();
			for (; itr != m_items.end(); itr++;) {
				if (strcmp((*itr)->Name(), name)) {
					break;
				}
			}

			if (itr != m_items.end()) {
				m_items.erase(itr);
			}
		}

		void DispatchEvent(
			iw::event& e)
		{
			for (T& t : m_items) {
				bool error = false;
				if (e.Category == IW::INPUT) {
					switch (e.Type) {
					case IW::MouseWheel:  t->On((IW::MouseWheelEvent&)e);  break;
					case IW::MouseMoved:  t->On((IW::MouseMovedEvent&)e);  break;
					case IW::MouseButton: t->On((IW::MouseButtonEvent&)e); break;
					case IW::Key:    	  t->On((IW::KeyEvent&)e);         break;
					case IW::KeyTyped:    t->On((IW::KeyTypedEvent&)e);    break;
					default: error = true;
					}
				}

				else if (e.Category == IW::WINDOW) {
					switch (e.Type) {
					case IW::Resized: t->On((IW::WindowResizedEvent&)e); break;
					case IW::Closed:  /*t.On((IW::WindowClosedEvent&)e;*/   break;
					default: error = true;
					}
				}

				if (error) {
					LOG_WARNING << "Layer Stack mishandled event " + e.Type << "!";
				}
			}
		}

		iterator begin() {
			return m_items.begin();
		}

		iterator end() {
			return m_items.end();
		}
	};
}
}
