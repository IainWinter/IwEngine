#pragma once

#include "set/sparse_set.h"

namespace iwecs {
	class icomponent_group {
	public:
		virtual ~icomponent_group() {}
	};

	template<
		typename... _components_t>
	class component_group;

	template<
		typename _component_t,
		typename... _components_t>
	class component_group<_component_t, _components_t...>
		: public icomponent_group
		, public component_group<_component_t>
		, public component_group<_components_t...>
	{
	public:
		template<
			typename _t>
		component_group<_t>* as() {
			return this;
		}
	};

	template<
		typename _component_t>
	class component_group<_component_t>
		: public icomponent_group
	{
	private:
		iwu::sparse_set<std::size_t, _component_t> _components;

	public:
		void insert(_component_t&& component) {
			_components.insert(0, std::forward<_component_t>(component));
		}
	};
}