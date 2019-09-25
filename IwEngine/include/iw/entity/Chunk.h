#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	struct IWENTITY_API Chunk {
		class iterator {
		private:
			char* Components;
		};

		using EntityComponentType = iwu::ref<Entity2>;

		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t EntityIndex;
		
		char Buffer[];

		size_t ReserveEntity(
			const EntityComponentType& entity);

		void FreeEntity(
			size_t index);

		EntityComponentType* GetEntity(
			size_t index);
	};
}
