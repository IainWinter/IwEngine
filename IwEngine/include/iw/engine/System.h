#pragma once

#include "iw/entity/Space.h"
#include "iw/graphics/Renderer.h"
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
		std::queue<size_t> m_delete; // Probly make it so space can queue component creation at the ComponentArray level because of templated bs

	protected:
		IwEntity::Space& Space;
		IW::Graphics::Renderer& Renderer;

		virtual void Update(
			IwEntity::EntityComponentArray& view) = 0;

		void QueueDestroyEntity(
			size_t index)
		{
			m_delete.push(index);
		}
	public:
		System(
			IwEntity::Space& space,
			IW::Graphics::Renderer& renderer,
			const char* name)
			: m_name(name)
			, Space(space)
			, Renderer(renderer)
		{}

		virtual ~System() {}

		virtual int Initialize() {
			return 0;
		}

		virtual void Destroy() {}

		void Update() override {
			IwEntity::EntityComponentArray eca = Space.Query<_cs...>();
			// Break up view into Viewlets to execute on seperate threads

			// Execute threads
			Update(eca);

			// Execute queues space operations
			while (!m_delete.empty()) {
				size_t index = m_delete.front() ;
				Space.DestroyEntity(index);
				m_delete.pop();
			}
		}

		inline const char* Name() {
			return m_name;
		}
	};	
}
