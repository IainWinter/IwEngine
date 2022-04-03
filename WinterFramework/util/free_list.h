#pragma once

#include <vector>
#include <limits>
#include <stddef.h>
#include <assert.h>

template<typename _t>
struct free_list
{
	// (begin, begin + size]
	struct free_range
	{
		_t m_begin;
		_t m_size;
	};

	using iterator = typename std::vector<free_range>::iterator;
	using const_iterator = typename std::vector<free_range>::const_iterator;

	std::vector<free_range> m_list;
	_t m_limit_begin;
	_t m_limit_size;

	free_list(_t begin = 0, _t size = std::numeric_limits<_t>::max())
	{
		m_limit_begin = begin;
		m_limit_size = size;
		reset();
	}

	void reset()
	{
		m_list.clear();
		unmark(m_limit_begin, m_limit_size);
	}

	void mark(_t begin, _t size)
	{
		assert(size > 0 && "Tried to mark a size of zero");
		assert(begin >= m_limit_begin && begin + size <= m_limit_begin + m_limit_size && "Mark is outside limits");

		int index = -1;

		for (int i = 0; i < m_list.size(); i++)
		{
			free_range& range = m_list.at(i);

			if (range.m_begin + range.m_size > begin) // we break once a range is found, so continue until the first past our begin
			{
				index = i;
				break;
			}
		}

		assert(index != -1 && "Mark already filled");  // this is if you try to mark something not past any free_range

		free_range& range = m_list.at(index);

		_t dist_from_begin = begin - range.m_begin;

		assert(dist_from_begin >= 0 && "Mark already filled"); // this is if you try to mark something not in the last free_range
		assert(size <= range.m_size - dist_from_begin && "Not enough free space at end of mark");

		if (dist_from_begin == 0) // if at start of range, shrink
		{
			if (range.m_size == size) // if filled range, remove
			{
				m_list.erase(m_list.begin() + index);
			}

			else // or shrink
			{
				range.m_size -= size;
				range.m_begin += size;
			}
		}

		else // or we need to split the range by inserting another one at the end to fill the gap between begin + size and range.m_begin + range.m_size
		{
			_t right_size = range.m_size - size - dist_from_begin;
			range.m_size = dist_from_begin;
			
			if (right_size > 0) // if marking the end of a free_range, don't split
			{
				m_list.insert(m_list.begin() + index + 1, free_range { begin + size, right_size });
			}
		}
	}

	void unmark(_t begin, _t size)
	{
		assert(size > 0 && "Tried to unmark a size of zero");

		if (m_list.size() == 0) // insert the first free zone if none exist
		{
			m_list.push_back(free_range { begin, size });
			return; // early exit
		}

		int index = -1;

		for (int i = 0; i < m_list.size(); i++)
		{
			free_range& range = m_list.at(i);

			if (range.m_begin + range.m_size > begin)
			{
				index = i;
				break;
			}
		}

		assert(index != -1 && "Mark already empty");

		free_range& range = m_list.at(index);

		assert(begin + size <= range.m_begin && "Unmark crossed an already unmaked region");

		free_range& range_left  = *m_list.insert(m_list.begin() + index, free_range { begin, size });
		free_range& range_right = m_list.at(index + 1); // + 1 because of insert

		if (range_left.m_begin + range_left.m_size == range_right.m_begin) // combine touching to the right
		{
			range_left.m_size += range_right.m_size;  // expand left
			m_list.erase(m_list.begin() + index + 1); // erase right
		}

		if (index - 1 >= 0) // try to combine left if it exists
		{
			free_range& more_left = m_list.at(index - 1);

			if (more_left.m_begin + more_left.m_size == range_left.m_begin) // combine touching to the left
			{
				more_left.m_size += range_left.m_size; // expand more left
				m_list.erase(m_list.begin() + index);  // erase current
			}
		}
	}

	// helpers

	_t mark_first(_t size)
	{
		_t begin = first_fits(size);
		mark(begin, size);
		return begin;
	}

	_t first_fits(_t size) const
	{
		for (const free_range& range : m_list)
		{
			if (range.m_size >= size)
			{
				return range.m_begin;
			}
		}

		assert(false && "No range is large enough to fit size");

		return _t(-1);
	}

	bool has_space(_t size) const
	{
		for (const free_range& range : m_list) if (range.m_size >= size) return true;
		return false;
	}

	bool is_marked(_t begin) const
	{
		for (const free_range& range : m_list) 
		{
			if (range.m_begin > begin) // if begin is past test, because list is sorted it has to be marked
			{
				break;
			}

			if (   range.m_begin <= begin
				&& begin < range.m_begin + range.m_size) // if in a free range
			{
				return false;
			} 
		}
		
		return true;
	}

	iterator begin() { return m_list.begin(); }
	iterator end() { return m_list.end(); }
	const_iterator begin() const { return m_list.begin(); }
	const_iterator end() const { return m_list.end(); }
};