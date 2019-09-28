#include "iw/entity/Chunk.h"

namespace IwEntity {
	Entity* Chunk::ReserveComponents() {
		Entity* entity = (Entity*)Buffer + CurrentIndex;
		++CurrentIndex;
		++Count;

		return entity;
	}

	void Chunk::FreeComponents() {
		--Count;
	}
}
