#pragma once

#include "IwEntity.h"
#include "Entity.h"

namespace IwEntity {
	struct Chunk {
		Chunk* Next;
		Chunk* Previous;

		size_t Count;
		size_t EntityIndex;
		size_t CurrentIndex;
		
		char Buffer[];

		Entity* ReserveComponents();

		void FreeComponents();
	};
}
