#include "iw/entity/Chunk.h"

namespace IwEntity {
	size_t Chunk::ReserveEntity(
		EntityComponentType entity)
	{
		EntityComponentType* ptr = GetEntity(Count);
		*ptr = entity;

		return Count++;
	}

	void Chunk::FreeEntity(
		size_t index)
	{
		EntityComponentType* entity = GetEntity(index);
		entity->reset();

		Count--;
	}

	Chunk::EntityComponentType* Chunk::GetEntity(
		size_t index)
	{
		return (EntityComponentType*)Buffer + index;
	}
}
