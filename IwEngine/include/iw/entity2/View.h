#pragma once

#include "ComponentArray.h"

namespace IwEntity2 {
	class View {
	private:
		ComponentArray* m_componentArrays;
		size_t          m_componentCount;

	public:
		View(
			ComponentArray* componentArrays,
			size_t componentCount);

		~View();

		template<
			typename S>
		S GetComponents() {

		}
	};
}
