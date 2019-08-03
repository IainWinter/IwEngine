#pragma once

#include "Core.h"
#include <vector>

namespace IwEngine {
	template<
		typename T>
	class Stack {
	public:
		using iterator = typename std::vector<T>::iterator;
	private:
		std::vector<T> m_items;

	public:
		Stack() = default;

		~Stack() {
			m_items.clear();
		}

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

		iterator begin() {
			return m_items.begin();
		}

		iterator end() {
			return m_items.end();
		}
	};
}
