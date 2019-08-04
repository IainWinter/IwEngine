#pragma once

#include "iw/entity/Space.h"
#include "iw/entity/View.h"
#include <queue>
#include <thread>
#include "Components/Model.h"

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
		std::queue<std::thread>      m_threads;
		std::queue<IwEntity::Entity> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs

	protected:
		IwEntity::Space&         Space;
		IwGraphics::RenderQueue& RenderQueue;

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
			IwGraphics::RenderQueue& renderQueue,
			const char* name)
			: m_name(name)
			, Space(space)
			, RenderQueue(renderQueue)
		{}

		virtual ~System() {}

		void Update() override {
			// Break up view into Viewlets to execute on seperate threads
			View view = Space.ViewComponents<_cs...>();

			// Execute threads
			Update(view);

			// Execute queues space operations
			while (!m_delete.empty()) {
				IwEntity::Entity& entity = m_delete.front();

				//auto m = m_space.GetComponent<Model>(entity);

				//for (size_t i = 0; i < m->MeshCount; i++) {
				//	delete m->Meshes[i].VertexArray;
				//	delete m->Meshes[i].IndexBuffer;
				//}

				Space.DestroyEntity(entity);
				m_delete.pop();
			}
		}

		inline const char* Name() {
			return m_name;
		}
	};
		
}
