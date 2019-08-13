#pragma once

#include "IwEntity.h"
#include "ComponentArray.h"

namespace IwEntity2 {
	class IWENTITY2_API View {
	public:
		class Iterator {
		private:
			std::vector<ComponentArray::Iterator> m_itrs;

		public:
			Iterator(
				std::vector<ComponentArray::Iterator>&& itrs);

			Iterator& operator++() {
				for (ComponentArray::Iterator itr : m_itrs) {
					itr++;
				}
			}

			template<
				typename S>
			S GetComponents() {
				for()
			}
		};
	private:
		const std::vector<ComponentArray*> m_componentArrays;

	public:
		View(
			std::vector<ComponentArray*>&& componentArrays);

		Iterator begin();
		Iterator end();
	};
}
