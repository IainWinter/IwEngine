#pragma once

#include "component/archetype.h"
#include "component/group.h"
#include "component/view.h"

namespace iwent {
	class isubsapce {
	public:
		virtual ~isubsapce() {}

		virtual bool is_similar(
			archetype archetype) const = 0;
	};

	template<
		typename... _components_t>
	class subspace
		: public isubsapce
	{
	private:
		archetype m_archetype;
		group<_components_t...> m_components;

	public:
		subspace(
			archetype archetype,
			const group<_components_t...>& components)
			: m_archetype(archetype)
			, m_components(components)
		{}

		bool is_similar(
			archetype archetype) const
		{
			return has_any(m_archetype, archetype);
		}
	};
}
