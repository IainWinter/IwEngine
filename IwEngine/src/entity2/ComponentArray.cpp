#include "iw/entity2/ComponentManager.h"

namespace IwEntity2 {
	ComponentManager::ComponentManager()
		: m_allocator(Chunk::ChunkSize * 2, Chunk::ChunkSize)
	{}

	void* ComponentManager::CreateComponents(
		Entity& entity)
	{
		Chunk* chunk = EnsureChunkWithEntity(entity);

		char* components = chunk->Buffer + chunk->Count;

		

		while (head != nullptr) {


			head = head->Next;
		}


		auto itr = m_entities.find(entity);
		if (itr == m_entities.end()) {
			void* components = m_pool.alloc();
			m_entities.emplace(entity, components);

			return components;
		}

		return nullptr;
	}

	bool ComponentManager::DestroyComponents(
		Entity& entity)
	{
		auto itr = m_entities.find(entity);
		if (itr != m_entities.end()) {
			void* ptr = m_entities.at(entity);
			m_entities.erase(entity);
			m_pool.free(ptr);

			return true;
		}

		return false;
	}

	void* ComponentManager::GetComponents(
		Entity& entity)
	{
		auto itr = m_entities.find(entity);
		if (itr != m_entities.end()) {
			return m_entities.at(entity);
		}

		return nullptr;
	}

	Chunk* ComponentManager::EnsureChunkWithEntity(
		Entity& entity)
	{
		Chunk* chunk = FindChunkWithEntity(entity);
		if (chunk == nullptr) {
			Chunk*  newChunk = (Chunk*)malloc(Chunk::ChunkSize);
			Chunk*& oldChunk = m_chunks.at(entity.Archetype);
			newChunk->Next = oldChunk;
			oldChunk       = newChunk;
		}

		return chunk;
	}

	Chunk* ComponentManager::FindChunkWithEntity(
		Entity& entity)
	{
		auto itr = m_chunks.find(entity.Archetype);
		if (itr != m_chunks.end()) {
			EntityIndex index = 0;
			Chunk*      chunk = m_chunks.at(entity.Archetype);
			while (chunk != nullptr) {
				if (entity.Index >= index && entity.Index < index + chunk->Capacity) {
					return chunk;
				}

				index += chunk->Capacity;
				chunk =  chunk->Next;
			}

		}

		return nullptr;
	}
}
