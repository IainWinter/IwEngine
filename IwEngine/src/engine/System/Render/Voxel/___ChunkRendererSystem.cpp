#include "iw/engine/Systems/Render/Voxel/___ChunkRendererSystem.h"
#include "iw/engine/Time.h"

namespace iw {
namespace Engine {
	void Chunk::Update(
		Mesh* mesh)
	{
		//vector3 NxNyNz = vector3(-1, -1, -1);
		//vector3 NxPyNz = vector3(-1,  1, -1);
		//vector3 NxNyPz = vector3(-1, -1,  1);
		//vector3 NxPyPz = vector3(-1,  1,  1);
		//vector3 PxNyNz = vector3( 1, -1, -1);
		//vector3 PxPyNz = vector3( 1,  1, -1);
		//vector3 PxNyPz = vector3( 1, -1,  1);
		//vector3 PxPyPz = vector3( 1,  1,  1);

		//std::vector<vector3>  Verts;
		//std::vector<vector2>  UVs;
		//std::vector<unsigned> Index;

		//for (int x = 0; x < SIZE; x++) {
		//	for (int y = 0; y < SIZE; y++) {
		//		for (int z = 0; z < SIZE; z++) {
		//			Voxel& voxel = Voxels[x][y][z];

		//			// check if vert is needed
		//			Verts.emplace_back(NxNyNz.x * x / SIZE, NxNyNz.y * y / SIZE, NxNyNz.z * z / SIZE);
		//			Verts.emplace_back(NxPyNz.x * x / SIZE, NxPyNz.y * y / SIZE, NxPyNz.z * z / SIZE);
		//			Verts.emplace_back(NxNyPz.x * x / SIZE, NxNyPz.y * y / SIZE, NxNyPz.z * z / SIZE);
		//			Verts.emplace_back(NxPyPz.x * x / SIZE, NxPyPz.y * y / SIZE, NxPyPz.z * z / SIZE);
		//			Verts.emplace_back(PxNyNz.x * x / SIZE, PxNyNz.y * y / SIZE, PxNyNz.z * z / SIZE);
		//			Verts.emplace_back(PxPyNz.x * x / SIZE, PxPyNz.y * y / SIZE, PxPyNz.z * z / SIZE);
		//			Verts.emplace_back(PxNyPz.x * x / SIZE, PxNyPz.y * y / SIZE, PxNyPz.z * z / SIZE);
		//			Verts.emplace_back(PxPyPz.x * x / SIZE, PxPyPz.y * y / SIZE, PxPyPz.z * z / SIZE);

		//			UVs.emplace_back(voxel.Coords.x + 0.1f, voxel.Coords.y);
		//			UVs.emplace_back(voxel.Coords.x + 0.1f, voxel.Coords.y);
		//			UVs.emplace_back(voxel.Coords.x, voxel.Coords.y + 0.1f);
		//			UVs.emplace_back(voxel.Coords.x, voxel.Coords.y);
		//			UVs.emplace_back(voxel.Coords.x + 0.1f, voxel.Coords.y);
		//			UVs.emplace_back(voxel.Coords.x + 0.1f, voxel.Coords.y);
		//			UVs.emplace_back(voxel.Coords.x, voxel.Coords.y + 0.1f);
		//			UVs.emplace_back(voxel.Coords.x, voxel.Coords.y);

		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE);
		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE + 1);
		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE + 2);

		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE);
		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE);
		//			Index.push_back(x + y * SIZE + z * SIZE * SIZE);
		//		}
		//	}
		//}

		//mesh->SetVertices(Verts.size(), Verts.data());
		//mesh->SetUVs     (UVs.size(), UVs.data());
		//mesh->SetIndices (Index.size(), Index.data());

		//mesh->GenNormals();
	}
	
	ChunkRendererSystem::ChunkRendererSystem()
		: System<Transform, Chunk, Mesh>("Chunk Renderer")
	{

	}

	int ChunkRendererSystem::Initialize() {
		//ref<Shader> shader = Asset->Load<Shader>("shaders/test.shader");

		//Entity entity = Space->CreateEntity<Transform, Chunk, Mesh>();

		//Transform* transform = entity.SetComponent<Transform>();
		//Chunk* chunk = entity.SetComponent<Chunk>();
		//Mesh* mesh = entity.SetComponent<Mesh>();

		//chunk->Voxels[0][0][0] = Voxel{ vector2(0, 0) };

		//chunk->Update(mesh);

		//mesh->SetMaterial(REF<Material>(shader));
		//mesh->Initialize(Renderer->Device);

		return 0;
	}

	void ChunkRendererSystem::Update(
		EntityComponentArray& eca)
	{

	}
}
}
