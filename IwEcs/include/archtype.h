#pragma once

#include <vector>
#include "chunk.h"
#include "IwUtil/identifiable.h"

namespace iwecs {
	template<typename... _ts>
	class archtype : public iwutil::identifiable {
	private:
		using tup_t = std::tuple<_ts...>;

		tup_t m_components;
	public:
		archtype(_ts&&... args)
			: m_components(std::forward<_ts&&>(args)...) {}

		tup_t& components() {
			return m_components;
		}
	};
}