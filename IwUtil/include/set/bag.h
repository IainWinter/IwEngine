#pragma once

#include "iwutil.h"
#include "sparse_set.h"
#include "type/type_group.h"

namespace iwutil {
	//Simple bag backed by sparse sets
	template<
		typename _index_t>
	class bag {
	protected:
		struct set_group_tag;

		template<typename _t>
		using set_t       = iwutil::sparse_set<_index_t, _t>;
		using set_group_t = iwutil::type_group<set_group_tag>;

		std::vector<iwutil::sparse_set<_index_t>*> m_sets;

	public:
		template<
			typename _t,
			typename... _args_t>
		void emplace(
				_index_t index,
				_args_t&&... args)
		{
			ensure_set<_t>().emplace(index, std::forward<_args_t>(args)...);
		}

		template<
			typename _t>
		void erase(
			_index_t index)
		{
			ensure_set<_t>().erase(index);
		}

		template<
			typename _t>
		_t& at(
			_index_t index)
		{
			return ensure_set<_t>().at(index);
		}

	private:
		template<
			typename _t>
		set_t<_t>& ensure_set() {
			int index = set_group_t::type<_t>;
			if (index >= m_sets.size()) {
				m_sets.push_back(new set_t<_t>());
				index = m_sets.size() - 1;
			}

			return static_cast<set_t<_t>&>(*m_sets[index]);
		}
	};
}