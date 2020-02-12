#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Texture.h"
#include "iw/log/logger.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"
#include <vector>

#include "assimp/pbrmaterial.h"
namespace iw {
namespace Graphics {
	void LoadTexture(
		AssetManager& asset,
		Material& material,
		aiMaterial* aimaterial,
		aiTextureType type,
		const char* name);

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

		std::vector<iw::ref<Material>> materials;
		if (scene->HasMaterials()) {
			for (size_t i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* aimaterial = scene->mMaterials[i];
				const char* ainame     = aimaterial->GetName().C_Str();

				Color albedo;
				Color ao;
				//Color emissiveColor;
				//float metallic;
				//float roughness = 0.2f;
				//float ambientOcclusion;
				
				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&albedo); 
				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&ao);
			//	aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&emissiveColor);

				//aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS, &metallic);

				//metallic /= 4 * 128; // obj files scale this by 4? and then opengl by 128???

				Material material;

				// Loaded  value
				// Scanned description
				// User value
				
				material.Set("albedo", albedo);
				material.Set("ao", ao);

				//material.SetFloats("emissiveColor", &emissiveColor, 4);
				//material.SetFloat("metallic", metallic);
				//material.SetFloat("roughness", roughness);

				if (aimaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0) {
					LOG_WARNING << "Unknown textures in material " << path;
				}

				LoadTexture(m_asset, material, aimaterial, aiTextureType_DIFFUSE,      "albedoMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_SPECULAR,     "specularMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_AMBIENT,      "aoMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_EMISSIVE,     "emissiveMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_NORMALS,      "normalMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_SHININESS,    "shininessMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_OPACITY,      "opacityMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_DISPLACEMENT, "displacementMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_LIGHTMAP,     "lightmapMap");
				LoadTexture(m_asset, material, aimaterial, aiTextureType_REFLECTION,   "reflectionMap");

				//size_t size = strlen(ainame) + 1;
				//char*  name = new char[size];
				//memcpy(name, ainame, size);

				materials.push_back(m_asset.Give<Material>(ainame, &material));
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

		Model* model = new Model {
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
				mesh.SetUVs(mesh.VertexCount, nullptr);
				for (size_t i = 0; i < mesh.VertexCount; i++) { // ai tex coords are vec3
					mesh.Uvs[i].x = aimesh->mTextureCoords[0][i].x;
					mesh.Uvs[i].y = aimesh->mTextureCoords[0][i].y;
				}
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

		//Mesh* mesh = new Mesh[];

		//Mesh* quad = new Mesh();
		//quad->SetMaterial(quadMaterial);
		//quad->SetVertices(model.VertexCount, qdata.Vertices);
		//quad->SetNormals(qdata.VertexCount, qdata.Normals);
		//quad->SetIndices(qdata.FaceCount, qdata.Faces);
		//quad->Compile(Renderer.Device);

		return model;
	}

	void LoadTexture(
		AssetManager& asset,
		Material& material,
		aiMaterial* aimaterial,
		aiTextureType type,
		const char* name)
	{
		aiString texturePath;
		unsigned count = aimaterial->GetTextureCount(type);
		for (unsigned i = 0; i < count; i++) {
			if (aimaterial->GetTexture(type, i, &texturePath) == AI_SUCCESS) {
				std::stringstream ss(name);
				if (i > 0) {
					ss << i;
				}

				iw::ref<Texture> texture = asset.Load<Texture>(texturePath.C_Str());
				if (texture) {
					material.SetTexture(ss.str(), texture);
				}
			}
		}
	}
}
}
