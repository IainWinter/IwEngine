#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		using EntityComponentType = std::weak_ptr<const Entity2>;

		Chunk* Next;
		Chunk* Previous;
		size_t Count;
		char Buffer[];

		size_t ReserveEntity(
			EntityComponentType entity);

		void FreeEntity(
			size_t index);

		EntityComponentType* GetEntity(
			size_t index);
	};
}
