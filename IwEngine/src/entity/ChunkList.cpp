#include "iw/entity/ChunkList.h"
#include <assert.h>

#ifdef IW_DEBUG
#	include "iw/log/logger.h"
#include "..\..\include\iw\entity\ChunkList.h"
#endif

namespace iw {
namespace ECS {
	using iterator = ChunkList::iterator;

	// replace with free list like thing but for valid entities 
	iterator& iterator::operator++() {
		do {
			Index++;
			if (Index == m_chunk->EndIndex()) {
				do {
					m_chunk = m_chunk->Next;
					if (m_chunk) {
						Index = m_chunk->BeginIndex();
					}
				} while (m_chunk && m_chunk->Count == 0);
			}
		} while (m_chunk && !m_chunk->GetEntity(Index)->Alive);

		// should calc the number of steps taken and add that * component size to the cdata ptrs
		// instead of multiplying everytime

		return *this;
	}

	bool iterator::operator==(
		const iterator& itr) const
	{
		return this->Index == itr.Index;
	}

	bool iterator::operator!=(
		const iterator& itr) const
	{
		return !operator==(itr);
	}

	EntityComponentData iterator::operator*() {
		for (size_t i = 0; i < m_indices.Count; i++)  // use raw pointers? // put this in ++
		{
			m_data.Components[i] = m_chunk->GetComponentPtr(
				m_archetype.Layout[m_indices.Indices[i]],
				Index
			);
		}

		EntityHandle* entity = m_chunk->GetEntity(Index);

		return EntityComponentData { *entity, entity->Index, entity->Version, m_data };
	}

	iterator::iterator(
		Chunk* chunk,
		size_t index,
		const Archetype& archetype,
		const std::vector<ref<Component>>& components,
		pool_allocator& componentPool
	)
		: m_chunk(chunk)
		, Index(index)
		, m_archetype(archetype)
	{
		size_t count = 0;
		for (ref<Component> component : components) {
			if (archetype.HasComponent(component)) {
				count++;
			}
		}

		m_indices.Count = count;
		m_indices.Indices = componentPool.alloc_ref_c<size_t>(count);
		m_data.Components = componentPool.alloc_ref_c<void*>(count);
		
		for (size_t i = 0; i < count;           i++)
		for (size_t j = 0; j < archetype.Count; j++) 
		{
			if (components[i]->Type == archetype.Layout[j].Component->Type) 
			{
				m_indices.Indices[i] = j;
				break;
			}
		}
	}

	ChunkList::ChunkList(
		const Archetype& archetype,
		pool_allocator& componentPool,
		pool_allocator& chunkPool
	)
		: m_root(nullptr)
		, m_count(0)
		, m_chunkCount(0)
		, m_archetype(archetype)
		, m_componentPool(componentPool)
		, m_chunkPool(chunkPool)
	{}

	size_t ChunkList::ReserveComponents(
		const EntityData& entityData)
	{
		Chunk& chunk = FindOrCreateChunk();
		size_t index = chunk.ReserveComponents();

		EntityHandle* entityComponent = chunk.GetEntity(index);
		*entityComponent = entityData.Entity;

		++m_count;

		return index;
	}

	bool ChunkList::ReinstateComponents(
		const EntityData& entityData)
	{
		Chunk* chunk = FindChunk(entityData.ChunkIndex);
		if (!chunk) {
			return false;
		}

		EntityHandle* entityComponent = chunk->GetEntity(entityData.ChunkIndex);
		if (entityComponent->Index != entityData.Entity.Index) {
			return false;
		}

		chunk->ReinstateComponents();

		*entityComponent = entityData.Entity;

		for (size_t i = 0; i < m_archetype.Count; i++) {
			ArchetypeLayout& layout = m_archetype.Layout[i];

			void* old = chunk->GetComponentPtr(layout, entityData.ChunkIndex);
			memset(old, 0, layout.Component->Size); // could put this in Chunk.h if it sounds better
		}

		++m_count;

		return true;
	}

	bool ChunkList::FreeComponents(
		const EntityData& entityData)
	{
		Chunk* chunk = FindChunk(entityData.ChunkIndex);
		if (!chunk) {
			return false;
		}

		chunk->FreeComponents();

		EntityHandle* entityComponent = chunk->GetEntity(entityData.ChunkIndex);
		entityComponent->Alive = false;

		for (size_t i = 0; i < m_archetype.Count; i++) {
			ArchetypeLayout& layout = m_archetype.Layout[i];
			void* component = chunk->GetComponentPtr(layout, entityData.ChunkIndex);
			
			layout.Component->DestructorFunc(component);
			memset(component, 0, layout.Component->Size); // could put this in Chunk.h if it sounds better
		}

		--m_count;

		// If chunk is empty free it
		if (chunk->Count == 0) 
		{	
			if (chunk->Previous) {
				chunk->Previous->Next = chunk->Next;
			}

			if (chunk->Next) {
				chunk->Next->Previous = chunk->Previous;
			}

			if (chunk == m_root) {
				m_root = m_root->Next;
			}

			LogChunkInfo(chunk, "-");

			--m_chunkCount;

			delete[] chunk->Buffer; // m_chunkPool.free(chunk->Buffer, GetChunkBufferSize(m_archetype, chunk->Capacity));
			delete chunk; // m_chunkPool.free<Chunk>(chunk);
		}

		return true;
	}

	bool ChunkList::MoveComponents(
		ChunkList& to,
		size_t index,
		size_t newIndex)
	{
		Chunk* chunk = FindChunk(index);
		if (!chunk) {
			return false;
		}

		for (size_t i = 0; i < m_archetype.Count; i++) {
			ArchetypeLayout& layout = m_archetype.Layout[i];
			ref<Component> component = layout.Component;

			void* ptr  = to.GetComponentPtr(component, newIndex);
			if (!ptr) continue;

			void* from = chunk->GetComponentPtr(layout, index);
			if (!from) continue; // not sure if this can ever trip

			component->DeepCopyFunc(ptr, from);
		}

		return true;
	}

	void* ChunkList::GetComponentPtr(
		const ref<Component>& component,
		size_t index)
	{
		Chunk* chunk = FindChunk(index);
		if (!chunk) {
			return false;
		}

		size_t i = 0;
		for (; i < m_archetype.Count; i++) {
			if (component->Type == m_archetype.Layout[i].Component->Type) {
				break;
			}
		}

		if (i == m_archetype.Count) {
			return nullptr;
		}

		return chunk->GetComponentPtr(m_archetype.Layout[i], index);
	}

	EntityHandle* ChunkList::GetEntity(
		size_t index)
	{
		Chunk* chunk = FindChunk(index);
		if (!chunk) {
			return nullptr;
		}

		return chunk->GetEntity(index);
	}

	int ChunkList::IndexOf(
		const ref<Component>& component,
		void* instance)
	{
		int index = -1;
		Chunk* chunk = m_root;
		const ArchetypeLayout* layout = m_archetype.GetLayout(component);

		if (layout)
		while (chunk) {
			index = chunk->IndexOf(*layout, instance);
			if (index == -1) {
				chunk = chunk->Next;
			}

			else {
				break;
			}
		}

		return index;
	}

	iterator ChunkList::Begin(
		const ComponentQuery& query)
	{
		Chunk* chunk = m_root;
		size_t index = 0;
		while (chunk && chunk->Count == 0) {
			chunk = chunk->Next;
		}
		
		if (chunk) {
			index = chunk->BeginIndex();
		}

		return iterator(m_root, index, m_archetype, query.GetComponents(), m_componentPool);
	}

	iterator ChunkList::End(
		const ComponentQuery& query)
	{
		Chunk* chunk = m_root;
		size_t index = 0;
		while (chunk && chunk->Next) {
			chunk = chunk->Next;
		}

		if (chunk) {
			index = chunk->EndIndex();
		}

		return iterator(nullptr, index, m_archetype, query.GetComponents(), m_componentPool);
	}

	Chunk* ChunkList::FindChunk(
		size_t index)
	{
		Chunk* chunk = m_root;
		while (chunk && !chunk->ContainsIndex(index)) {
			chunk = chunk->Next;
		}

		return chunk;
	}

	Chunk* ChunkList::CreateChunk(
		Chunk* previous)
	{
		size_t capacity = pow(2, m_chunkCount);
		size_t indexOff = previous ? previous->IndexOffset + previous->Capacity : 0;

		Chunk* chunk = new Chunk(); //m_chunkPool.alloc<Chunk>();
		chunk->Buffer = new char[GetChunkBufferSize(m_archetype, capacity)];// m_chunkPool.alloc<char>(GetChunkBufferSize(m_archetype, capacity));
		chunk->Capacity = capacity;
		chunk->IndexOffset = indexOff;
		chunk->Previous = previous;

		LogChunkInfo(chunk, "+");

		++m_chunkCount;

		return chunk;
	}

	Chunk& ChunkList::FindOrCreateChunk() {
		if (!m_root) {
			m_root = CreateChunk(nullptr);
		}

		Chunk* chunk = m_root;
		while (chunk && chunk->CurrentIndex == chunk->Capacity) {
			if (chunk->Next == nullptr) {
				chunk->Next = CreateChunk(chunk);
			}

			chunk = chunk->Next;
		}

		return *chunk;
	}

	size_t ChunkList::GetChunkBufferSize(
		const Archetype& archetype,
		size_t capasity)
	{
		return capasity * (archetype.Size + sizeof(EntityHandle));
	}
}
}
