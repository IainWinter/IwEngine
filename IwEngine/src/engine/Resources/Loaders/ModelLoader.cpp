#include "iw/engine/Resources/Loaders/ModelLoader.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include <string>

namespace IwEngine {
	Model* ModelLoader::Load(
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

		Model* model = new Model {
			new Mesh[scene->mNumMeshes],
			new FaceIndex[scene->mNumMeshes],
			scene->mNumMeshes
		};

		for (size_t i = 0; i < scene->mNumMeshes; ++i) {
			const aiMesh* aimesh = scene->mMeshes[i];
			constexpr size_t FACE_SIZE = 3 * sizeof(size_t);
			constexpr size_t VERT_SIZE = sizeof(Vertex);
			constexpr size_t VEC3_SIZE = sizeof(iwm::vector3);

			model->Indices[0].FaceCount = 0;
			model->Indices[0].Faces = (size_t*)malloc(FACE_SIZE * aimesh->mNumFaces);

			size_t* faces = model->Indices[0].Faces;
			size_t& faceCount = model->Indices[0].FaceCount;

			for (size_t t = 0; t < aimesh->mNumFaces; ++t) {
				const aiFace* face = &aimesh->mFaces[t];
				memcpy(&faces[faceCount], face->mIndices, FACE_SIZE);
				faceCount += 3;
			}

			Mesh& mesh = model->Meshes[i];

			mesh.VertexCount = aimesh->mNumVertices;
			mesh.Vertices = (Vertex*)malloc(VERT_SIZE * mesh.VertexCount);

			for (size_t i = 0; i < mesh.VertexCount; i++) {
				memcpy(&mesh.Vertices[i].Vertex, &aimesh->mVertices[i], VEC3_SIZE);
				memcpy(&mesh.Vertices[i].Normal, &aimesh->mNormals[i], VEC3_SIZE);
			}
		}

		importer.FreeScene();

		loaded.push_back(model);

		return model;
	}

	void ModelLoader::Release(
		Model* resource)
	{
		delete resource;
	}
}
