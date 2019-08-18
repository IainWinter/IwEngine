#pragma once

#include "iw/entity2/EntityArchetype.h"

namespace IwEntity2 {
	// Struct for accessing data in chunks
	struct Chunk {
		Chunk* Next;  // 4 | 8

		size_t Count;  // 8 | 16
		size_t Capacity; // 12 | 24

		char Buffer[]; // Start of buffer

		const static size_t ChunkSize;        //16 Kb
		const static size_t ChunkHeaderSize; // Size of Chunk without buffer
		const static size_t BufferSize;      // Size of Chunk without header

		// Mad functions for getting streams and diffrent data from the buffer

		// First component is all the entitie, so that will be at the beginning of every chunk
		// Components will be layed out by type ordered by their respectul entity's index

		static size_t EntityCapacity(
			const Archetype& archetype);

		static Chunk* MakeChunk(
			const Archetype& archetype,
			Chunk* next = nullptr);

		char* GeStream(
			const Archetype& archetype);
	};
}
