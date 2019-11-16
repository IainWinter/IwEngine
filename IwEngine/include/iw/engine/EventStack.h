#pragma once

#include "iw/events/event.h"
#include <vector>

namespace IwEngine {
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
			T* item)
		{
			iterator itr = std::find(begin(), end(), layer);
			m_items.erase(itr);
		}

		void PublishEvent(
			iw::event& event)
		{
			for (T& t : m_items) {
				
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
