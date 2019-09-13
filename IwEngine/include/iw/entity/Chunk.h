#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		using EntityComponentType = iwu::weak<const Entity2>;

		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t EntityIndex;
		
		char Buffer[];

		size_t ReserveEntity(
			EntityComponentType entity);

		void FreeEntity(
			size_t index);

		EntityComponentType* GetEntity(
			size_t index);
	};
}
