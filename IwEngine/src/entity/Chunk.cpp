#include "iw/entity/Chunk.h"

namespace IwEntity {
	size_t Chunk::ReserveEntity(
		std::weak_ptr<Entity2> entity)
	{
		Entity2* ptr = GetEntity(Count);
		*ptr = *entity.lock();

		return Count++;
	}

	bool Chunk::FreeEntity(
		size_t index)
	{
		Entity2* entity = GetEntity(index);

		Count--;

		return entity;
	}

	Entity2* Chunk::GetEntity(
		size_t index)
	{
		return (Entity2*)Buffer + index;
	}
}
