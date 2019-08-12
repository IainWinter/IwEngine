#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"

namespace IwEntity2 {
	class View {
	private:
		using Iterator = ComponentArray::iterator;

		Iterator* m_componentArrays;
		size_t    m_componentCount;

	public:
		View(
			Iterator* componentArrays,
			Component* componentIds,
			size_t componentArrayCount,
			size_t componentCount);

		~View();

		template<
			typename S>
		S GetComponents() {

			//
		}
	};
}
