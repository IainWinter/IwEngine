#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>

namespace IwGraphics {
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

		ModelData* model = new ModelData{
			new MeshData[scene->mNumMeshes],
			new FaceIndex[scene->mNumMeshes],
			scene->mNumMeshes
		};

		for (size_t i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* aimesh = scene->mMeshes[i];
			constexpr size_t FACE_SIZE = 3 * sizeof(size_t);
			constexpr size_t VERT_SIZE = sizeof(Vertex);
			constexpr size_t VEC3_SIZE = sizeof(iwm::vector3);

			model->Indices[i].FaceCount = 0;
			model->Indices[i].Faces = (size_t*)malloc(FACE_SIZE * aimesh->mNumFaces);

			size_t* faces = model->Indices[i].Faces;
			size_t& faceCount = model->Indices[i].FaceCount;

			if (faces) {
				for (size_t t = 0; t < aimesh->mNumFaces; ++t) {
					const aiFace* face = &aimesh->mFaces[t];
					memcpy(&faces[faceCount], face->mIndices, FACE_SIZE);
					faceCount += 3;
				}
			}

			MeshData& mesh = model->Meshes[i];

			mesh.VertexCount = aimesh->mNumVertices;
			mesh.Vertices = (Vertex*)malloc(VERT_SIZE * mesh.VertexCount);

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
		delete resource;
	}
}
