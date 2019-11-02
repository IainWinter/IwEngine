#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Texture.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"
#include <vector>

namespace IW {
	MeshLoader::MeshLoader(
		AssetManager& asset)
		: AssetLoader(asset)
	{}

	Model* MeshLoader::LoadAsset(
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

		std::vector<iwu::ref<IW::Material>> materials;
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

				metallic /= 4 * 128; // obj files scale this by 4? and then opengl by 128???

				IW::Material material;
				//material.SetFloats("albedo", &albedo, 3);
				//material.SetFloats("emissiveColor", &emissiveColor, 4);
				material.SetFloat("metallic", metallic);
				material.SetFloat("roughness", roughness);

				aiString texturePath;
				if (   aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0
					&& aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
				{
					texturePath.Set(std::string(texturePath.C_Str()));

					iwu::ref<IW::Texture> texture = m_asset.Load<IW::Texture>(texturePath.C_Str());
					material.SetTexture("albedoMap", texture);
				}

				size_t size = strlen(ainame) + 1;
				char*  name = new char[size];
				memcpy(name, ainame, size);

				materials.push_back(m_asset.Give<IW::Material>(name, &material));
			}
		}

		//if (scene->HasTextures()) { // embedded textures?
		//	aiTexture* texture = scene->mTextures[0];
		//}

		Model* model = new Model{
			new Mesh[scene->mNumMeshes],
			scene->mNumMeshes
		};

		for (size_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* aimesh = scene->mMeshes[i];

			size_t indexBufferSize = 0;
			for (size_t t = 0; t < aimesh->mNumFaces; t++) {
				indexBufferSize += aimesh->mFaces[t].mNumIndices; // could a face have diffrent incices?
			}

			Mesh& mesh = model->Meshes[i];

			mesh.SetMaterial(materials.at(aimesh->mMaterialIndex));

			mesh.IndexCount = 0;
			mesh.Indices = new unsigned int[indexBufferSize];

			if (mesh.Indices) {
				for (size_t t = 0; t < aimesh->mNumFaces; t++) {
					const aiFace& face = aimesh->mFaces[t];
					for (size_t f = 0; f < face.mNumIndices; f++) {
						mesh.Indices[mesh.IndexCount] = face.mIndices[f];
						mesh.IndexCount++;
					}
				}
			}

			//IW::Mesh* mesh = root;

			//scene->mMeshes[0]->

			mesh.VertexCount = aimesh->mNumVertices;
			if (aimesh->HasPositions()) {
				mesh.Vertices = new iwm::vector3[mesh.VertexCount];
				memcpy(mesh.Vertices, aimesh->mVertices, mesh.VertexCount * sizeof(iwm::vector3));
			}

			if (aimesh->HasNormals()) {
				mesh.Normals = new iwm::vector3[mesh.VertexCount];
				memcpy(mesh.Normals, aimesh->mNormals, mesh.VertexCount * sizeof(iwm::vector3));
			}

			// Need to check for multiple channels i guess
			if (aimesh->HasTextureCoords(0)) {
				mesh.Uvs = new iwm::vector2[mesh.VertexCount];
				memcpy(mesh.Uvs, aimesh->mTextureCoords[0], mesh.VertexCount * sizeof(iwm::vector2));
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
