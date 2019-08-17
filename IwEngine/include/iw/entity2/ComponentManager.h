#pragma once

#include "IwEntity.h"
#include "Chunk.h"
#include "Entity.h"
#include "Archetype.h"
#include <unordered_map>

namespace IwEntity2 {
	class IWENTITY2_API ComponentManager {
	private:
		std::unordered_map<EntityArchetype, Chunk*> m_chunks;
		// linked list allocator

	public:
		ComponentManager();

		void* CreateComponents(
			Entity& entity);

		bool DestroyComponents(
			Entity& entity);

		void* GetComponents(
			Entity& entity);
	private:
		Chunk* EnsureChunkWithEntity(
			Entity& entity);

		Chunk* FindChunkWithEntity(
			Entity& entity);
	};
}
