#pragma once

#include "iw/entity/Space.h"
#include "iw/entity/View.h"
#include <queue>
#include <thread>


namespace IwEngine {
	class ISystem {
	public:
		virtual void Update() = 0;
	};

	template<
		typename... _cs>
	class System
		: public ISystem
	{
	private:
		const char*                  m_name;
		IwEntity::Space&             m_space;
		std::queue<std::thread>      m_threads;
		std::queue<IwEntity::Entity> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs

	protected:
		using View = IwEntity::View<_cs...>;

		virtual void Update(
			View& view) = 0;

		void QueueDestroyEntity(
			const IwEntity::Entity& entity)
		{
			m_delete.push(entity);
		}
	public:
		System(
			IwEntity::Space& space,
			const char* name)
			: m_name(name)
			, m_space(space)
		{}

		virtual ~System() {}

		void Update() override {
			// Break up view into Viewlets to execute on seperate threads
			View view = m_space.ViewComponents<_cs...>();

			// Execute threads
			Update(view);

			// Execute queues space operations
			while (!m_delete.empty()) {
				IwEntity::Entity& entity = m_delete.front();
				m_space.DestroyEntity(entity);
				m_delete.pop();
			}
		}

		inline const char* Name() {
			return m_name;
		}
	};
		
}
