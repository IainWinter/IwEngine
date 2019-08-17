#pragma once

namespace IwEntity2 {
	// Struct for accessing data in chunks
	struct Chunk {
		Chunk* Next;

		size_t Count;
		size_t Capacity;

		char Buffer[4];

		const static size_t ChunkSize = 16 * 1024;


		// Mad functions for getting streams and diffrent data from the buffer

		// First component is all the entitie, so that will be at the beginning of every chunk
		// Components will be layed out by type ordered by their respectul entity's index
	};
}
