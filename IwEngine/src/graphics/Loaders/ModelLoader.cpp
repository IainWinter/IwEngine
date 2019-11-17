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

		std::vector<iw::ref<IW::Material>> materials;
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
				//material.SetFloat("metallic", metallic);
				//material.SetFloat("roughness", roughness);

				aiString texturePath;
				if (   aimaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0
					&& aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
				{
					texturePath.Set(std::string(texturePath.C_Str()));

					iw::ref<IW::Texture> texture = m_asset.Load<IW::Texture>(texturePath.C_Str());
					material.SetTexture("albedoMap", texture);
				}

				size_t size = strlen(ainame) + 1;
				char*  name = new char[size];
				memcpy(name, ainame, size);

				materials.push_back(m_asset.Give<IW::Material>(name, &material));
			}
		}

		//if (scene->HasAnimations()) {
		//	for (size_t i = 0; i < scene->mNumAnimations; i++) {
		//		aiAnimation* aianimation = scene->mAnimations[i];

		//		aianimation->mMorphMeshChannels

		//		//aiNodeAnim* ainodeanim = aianimation->m

		//		//aianimation->
		//	}
		//}

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
				mesh.SetVertices(mesh.VertexCount, (iw::vector3*)aimesh->mVertices);
			}

			if (aimesh->HasNormals()) {
				mesh.SetNormals(mesh.VertexCount, (iw::vector3*)aimesh->mNormals);
			}

			if (aimesh->HasTangentsAndBitangents()) {
				mesh.SetTangents  (mesh.VertexCount, (iw::vector3*)aimesh->mTangents);
				mesh.SetBiTangents(mesh.VertexCount, (iw::vector3*)aimesh->mBitangents);
			}

			// Need to check for multiple channels i guess
			if (aimesh->HasVertexColors(0)) {
				mesh.SetColors(mesh.VertexCount, (iw::vector4*)aimesh->mColors[0]);
			}

			if (aimesh->HasTextureCoords(0)) {
				mesh.SetUVs(mesh.VertexCount, (iw::vector2*)aimesh->mTextureCoords[0]);
			}

			if (aimesh->HasBones()) {
				aiBone* aibone = aimesh->mBones[i];
				
				aiVector3D pos;
				aiVector3D rot;
				aiVector3D scale;

				aibone->mOffsetMatrix.Decompose(scale, rot, pos);

				Bone bone;
				bone.Offset.Position = iw::vector3(pos.x,     pos.y,   pos.z);
				bone.Offset.Scale    = iw::vector3(scale.x, scale.y, scale.z);
				bone.Offset.Rotation = iw::quaternion::from_euler_angles(rot.x, rot.y, rot.z);

				bone.WeightCount = aibone->mNumWeights;
				bone.Weights     = new VertexWeight[bone.WeightCount];

				for (size_t i = 0; i < bone.WeightCount; i++) {
					aiVertexWeight aiweight = aibone->mWeights[i];

					bone.Weights[i].Index  = aiweight.mVertexId;
					bone.Weights[i].Weight = aiweight.mWeight;
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
