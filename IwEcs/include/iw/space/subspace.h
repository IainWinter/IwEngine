#pragma once

#include "component/archetype.h"
#include "component/group.h"
#include "component/view.h"

namespace iwent {
	class isubspace {
	private:
		archetype m_archetype;

	public:
		isubspace(
			archetype archetype)
			: m_archetype(archetype)
		{}

		virtual ~isubspace() {}

		bool is_similar(
			archetype archetype) const
		{
			return has_any(m_archetype, archetype);
		}
	};

	template<
		typename... _components_t>
	class subspace
		: virtual public isubspace
	{
	private:
		group<_components_t...> m_components;

	public:
		subspace(
			archetype archetype,
			const group<_components_t...>& components)
			: isubspace(archetype)
			, m_components(components)
		{}
	};
}
