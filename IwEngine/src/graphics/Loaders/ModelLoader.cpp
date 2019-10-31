#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Mesh.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"

namespace IW {
	MeshLoader::MeshLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	ModelData* MeshLoader::LoadAsset(
		std::string path)
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

		std::vector<char*> materials(scene->mNumMaterials);
		if (scene->HasMaterials()) {
			for (size_t i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* aimaterial = scene->mMaterials[i];
				const char* ainame     = aimaterial->GetName().C_Str();

				Color albedo;
				//Color emissiveColor;
				float metallic;
				float roughness = 0.2f;
				//float ambientOcclusion;

				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE,  (aiColor4D*)&albedo);
			//	aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&emissiveColor);

				aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS, &metallic);
				//aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS_STRENGTH, &roughness);
				//aiGetMaterialFloat(aimaterial, AI_MATKEY_, &specular.a);

				metallic /= 4 * 128;
				//roughness /= 4;

				IW::Material material;
				material.SetFloats("albedo", &albedo, 3);
				//material.SetFloats("emissiveColor", &emissiveColor, 4);
				material.SetFloat("metallic", metallic);
				material.SetFloat("roughness", roughness);


				size_t size = strlen(ainame) + 1;
				char*  name = new char[size];
				memcpy(name, ainame, size);

				materials.push_back(name);

				m_asset.Give<IW::Material>(name, &material);
			}
		}

		ModelData* model = new ModelData{
			new MeshData[scene->mNumMeshes],
			scene->mNumMeshes
		};

		//IW::Mesh* root = new IW::Mesh();
		for (size_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* aimesh = scene->mMeshes[i];

			size_t indexBufferSize = 0;
			for (size_t t = 0; t < aimesh->mNumFaces; t++) {
				indexBufferSize += aimesh->mFaces[t].mNumIndices; // could a face have diffrent incices?
			}

			MeshData& mesh = model->Meshes[i];

			mesh.FaceCount = 0;
			mesh.Faces = new unsigned int[indexBufferSize];

			if (mesh.Faces) {
				for (size_t t = 0; t < aimesh->mNumFaces; t++) {
					const aiFace& face = aimesh->mFaces[t];
					for (size_t f = 0; f < face.mNumIndices; f++) {
						mesh.Faces[mesh.FaceCount] = face.mIndices[f];
						mesh.FaceCount++;
					}
				}
			}

			//IW::Mesh* mesh = root;

			//scene->mMeshes[0]->

			mesh.VertexCount = aimesh->mNumVertices;
			mesh.Vertices = new iwm::vector3[mesh.VertexCount];
			mesh.Normals  = new iwm::vector3[mesh.VertexCount];

			if (mesh.Vertices) {
				for (size_t i = 0; i < mesh.VertexCount; i++) {
					mesh.Vertices[i].x = aimesh->mVertices[i].x;
					mesh.Vertices[i].y = aimesh->mVertices[i].y;
					mesh.Vertices[i].z = aimesh->mVertices[i].z;

					mesh.Normals[i].x = aimesh->mNormals[i].x;
					mesh.Normals[i].y = aimesh->mNormals[i].y;
					mesh.Normals[i].z = aimesh->mNormals[i].z;
				}
			}
		}

		importer.FreeScene();

		//IW::Mesh* mesh = new IW::Mesh[];

		//IW::Mesh* quad = new IW::Mesh();
		//quad->SetMaterial(quadMaterial);
		//quad->SetVertices(model.VertexCount, qdata.Vertices);
		//quad->SetNormals(qdata.VertexCount, qdata.Normals);
		//quad->SetIndices(qdata.FaceCount, qdata.Faces);
		//quad->Compile(Renderer.Device);

		return model;
	}
}
