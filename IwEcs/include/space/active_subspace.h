#pragma once

#include "subspace.h"
#include "action.h"

namespace iwent {
	class iactive_subsapce
		: virtual public isubspace
	{
	public:
		iactive_subsapce(
			archetype archetype)
			: isubspace(archetype)
		{}

		virtual ~iactive_subsapce() {}

		virtual void add_action(
			iwutil::iaction* action) = 0;

		virtual void take_action() = 0;
	};

	template<
		typename... _components_t>
	class active_subspace
		: public iactive_subsapce
		, public subspace<_components_t...>
	{
	private:
		std::vector<iwutil::iaction*> m_actions;

	public:
		active_subspace(
			archetype archetype,
			const group<_components_t...>& components)
			: isubspace(archetype)
			, iactive_subsapce(archetype)
			, subspace<_components_t...>(archetype, components)
		{}

		~active_subspace() {
			for (iwutil::iaction* a : m_actions) {
				delete a;
			}

			m_actions.clear();
		}

		void add_action(
			iwutil::iaction* action) override
		{
			m_actions.push_back(action);
		}

		void take_action() override {
			for (iwutil::iaction* a : m_actions) {
				a->take();
			}
		}
	};
}
