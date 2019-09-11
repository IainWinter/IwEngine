#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;
		size_t Count;
		char Buffer[];

		size_t ReserveEntity(
			std::weak_ptr<Entity2> entity);

		bool FreeEntity(
			size_t index);

		Entity2* GetEntity(
			size_t index);
	};
}
