#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include <queue>
//#include <thread>

namespace IwEngine {
	class ISystem {
	public:
		virtual int  Initialize() = 0;
		virtual void Destroy() = 0;
		virtual void Update() = 0;
	};

	template<
		typename... _cs>
	class System
		: public ISystem
	{
	private:
		const char*                  m_name;
		//std::queue<std::thread>      m_threads;
		std::queue<IwEntity::Entity> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs

	protected:
		IwEntity::Space&         Space;
		IwGraphics::RenderQueue& RenderQueue;

		using View = IwEntity::EntityComponentArray;

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
			IwGraphics::RenderQueue& renderQueue,
			const char* name)
			: m_name(name)
			, Space(space)
			, RenderQueue(renderQueue)
		{}

		virtual ~System() {}

		virtual int Initialize() {
			return 0;
		}

		virtual void Destroy() {}

		void Update() override {
			IwEntity::EntityComponentArray view = Space.Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			Update(view);

			// Execute queues space operations
			while (!m_delete.empty()) {
				IwEntity::Entity& entity = m_delete.front();
				//Space.DestroyEntity(entity);
				m_delete.pop();
			}
		}

		inline const char* Name() {
			return m_name;
		}
	};
		
}
