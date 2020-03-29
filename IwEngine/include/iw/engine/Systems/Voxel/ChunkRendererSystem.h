#pragma once

#include "iw/engine/Core.h"
#include "iw/engine/System.h"
#include "iw/graphics/Scene.h"
#include "iw/graphics/Mesh.h"
#include <vector>

namespace iw {
namespace Engine {
	struct Voxel {
		iw::vector2 Coords;
	};

	struct Chunk {
		constexpr static int SIZE = 2;

		Voxel Voxels[SIZE][SIZE][SIZE];

		IWENGINE_API
		void Update(
			Mesh* mesh);
	};

	class ChunkRendererSystem
		: public System<Transform, Chunk, Mesh>
	{
	private:
		Scene* m_scene;

	public:
		struct Components {
			Transform* Transform;
			Chunk* Chunk;
			Mesh*  Mesh;
		};

	public:
		IWENGINE_API
		ChunkRendererSystem();

		IWENGINE_API
		int Initialize() override;

		IWENGINE_API
		void Update(
			EntityComponentArray& view) override;
	};
}

	using namespace Engine;
}
