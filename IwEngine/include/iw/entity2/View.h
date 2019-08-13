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
		};
	private:
		const std::vector<ComponentArray*> m_componentArrays;

	public:
		View(
			std::vector<ComponentArray*>&& componentArrays);

		Iterator begin();
		Iterator end();
		//template<
		//	typename S>
		//S GetComponents() {
		//	//
		//}
	};
}
