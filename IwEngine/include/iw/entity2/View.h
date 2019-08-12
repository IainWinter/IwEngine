#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"

namespace IwEntity2 {
	class IWENTITY2_API View {
	private:
		using Iterator = ComponentArray::Iterator;

		Iterator* m_componentArrays;
		size_t    m_componentCount;

	public:
		View(
			std::initializer_list<ComponentArray*> componentArrays);

		~View();

		template<
			typename S>
		S GetComponents() {
			//
		}
	};
}
