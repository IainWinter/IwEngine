#pragma once

#include "iw/events/event.h"
#include <vector>
#include <string_view>

namespace iw {
namespace Engine {
	// T needs to have a 'const char* Name()' function
	// and bool 'On(Event&)' functions for every event
	template<
		typename T>
	struct EventStack {
		using iterator = typename std::vector<T>::iterator;

		std::vector<T> m_items;

		void PushBack(
			const T& item)
		{
			m_items.push_back(item);
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
			for (; itr != m_items.end(); itr++) {
				if (strcmp((*itr)->Name(), name) == 0) {
					break;
				}
			}

			if (itr != m_items.end()) {
				m_items.erase(itr);
			}
		}

		void Pop(
			const T& item)
		{
			auto itr = m_items.begin();
			for (; itr != m_items.end(); itr++) {
				if (*itr == item) {
					break;
				}
			}

			if (itr != m_items.end()) {
				m_items.erase(itr);
			}
		}

		bool Contains(
			const T& item) const
		{
			return std::find(m_items.begin(), m_items.end(), item) != m_items.end();
		}

		T Get(
			std::string_view name)
		{
			for (auto itr = m_items.begin(); itr != m_items.end(); itr++) {
				if ((*itr)->Name() == name) {
					return *itr;
				}
			}

			return nullptr;
		}

		void DispatchEvent(
			iw::event& e)
		{
			bool error = false;

			switch (e.Group) {
				case iw::val(EventGroup::ACTION): {
					e.Handled = On((ActionEvent&)e);
					break;
				}
				case iw::val(EventGroup::INPUT): {
					switch (e.Type) {
						case InputEventType::MouseWheel:  e.Handled = On((MouseWheelEvent&) e); break;
						case InputEventType::MouseMoved:  e.Handled = On((MouseMovedEvent&) e); break;
						case InputEventType::MouseButton: e.Handled = On((MouseButtonEvent&)e); break;
						case InputEventType::Key:    	  e.Handled = On((KeyEvent&)        e); break;
						case InputEventType::KeyTyped:    e.Handled = On((KeyTypedEvent&)   e); break;
						case InputEventType::Command:     /* handle command event in layers!*/  break;
						default: error = true;
					}
					break;
				}
				case iw::val(EventGroup::OS): {
					e.Handled = On((OsEvent&)e);
					break;
				}
				case iw::val(EventGroup::WINDOW): {
					switch (e.Type) {
						case WindowEventType::Resized: e.Handled = On((WindowResizedEvent&)e); break;
						case WindowEventType::Closed:  /*t.On((WindowClosedEvent&)e;*/   break;
						default: error = true;
					}
					break;
				}
				case iw::val(EventGroup::PHYSICS): {
					switch (e.Type) {
						case PhysicsEventType::Collision: e.Handled = On((CollisionEvent&)e); break;
						default: error = true;
					}
					break;
				}
				case iw::val(EventGroup::ENTITY): {
					switch (e.Type) {
						case EntityEventType::Created:   e.Handled = On((EntityCreatedEvent&)  e); break;
						case EntityEventType::Destroy:   e.Handled = On((EntityDestroyEvent&)  e); break;
						case EntityEventType::Destroyed: e.Handled = On((EntityDestroyedEvent&)e); break;
						case EntityEventType::Moved:     e.Handled = On((EntityMovedEvent&)    e); break;
						default: error = true;
					}
					break;
				}
				default: error = true;
			}

			if (error) {
				LOG_WARNING << "Layer Stack mishandled event " + e.Type << "!";
			}
		}

		// Input events

		template<
			typename _e>
		bool On(
			_e& e)
		{
			for (T& t : m_items) {
				if (t->On(e)) {
					return true;
				}
			}

			return false;
		}

		size_t size() const {
			return m_items.size();
		}

		iterator begin() {
			return m_items.begin();
		}

		iterator end() {
			return m_items.end();
		}
	};
}

	using namespace Engine;
}
