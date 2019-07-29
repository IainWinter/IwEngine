#include "iw/engine/Loaders/ModelLoader.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>

namespace IwEngine {
	ModelData* ModelLoader::Load(
		const char* path)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(path,
			aiProcess_CalcTangentSpace
			| aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_SortByPType
		    | aiProcess_GenNormals);

		if (!scene) {
			LOG_WARNING << importer.GetErrorString();
			return nullptr;
		}

		ModelData* model = new ModelData {
			new MeshData[scene->mNumMeshes],
			scene->mNumMeshes
		};

		for (size_t i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* aimesh = scene->mMeshes[i];
			constexpr size_t FACE_SIZE = 3 * sizeof(size_t);
			constexpr size_t VERT_SIZE = sizeof(IwGraphics::Vertex);
			constexpr size_t VEC3_SIZE = sizeof(iwm::vector3);

			MeshData& mesh = model->Meshes[i];
			
			mesh.FaceCount = 0;
			mesh.Faces = new unsigned int[aimesh->mNumFaces];

			if (mesh.Faces) {
				for (size_t t = 0; t < aimesh->mNumFaces; ++t) {
					const aiFace* face = &aimesh->mFaces[t];
					memcpy(&mesh.Faces[mesh.FaceCount], face->mIndices, FACE_SIZE);
					mesh.FaceCount += 3;
				}
			}

			mesh.VertexCount = aimesh->mNumVertices;
			mesh.Vertices = new IwGraphics::Vertex[mesh.VertexCount];

			if (mesh.Vertices) {
				for (size_t i = 0; i < mesh.VertexCount; i++) {
					mesh.Vertices[i].Vertex.x = aimesh->mVertices[i].x;
					mesh.Vertices[i].Vertex.y = aimesh->mVertices[i].y;
					mesh.Vertices[i].Vertex.z = aimesh->mVertices[i].z;

					mesh.Vertices[i].Normal.x = aimesh->mNormals[i].x;
					mesh.Vertices[i].Normal.y = aimesh->mNormals[i].y;
					mesh.Vertices[i].Normal.z = aimesh->mNormals[i].z;
				}
			}
		}

		importer.FreeScene();

		loaded.push_back(model);

		return model;
	}

	void ModelLoader::Release(
		ModelData* resource)
	{
		auto itr = std::find(loaded.begin(), loaded.end(), resource);
		loaded.erase(itr);
	}
}
