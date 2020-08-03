#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/graphics/Bone.h"
#include "iw/graphics/Mesh.h"
#include "iw/graphics/Texture.h"
#include "iw/log/logger.h"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "assimp/pbrmaterial.h"
#include <vector>

namespace iw {
namespace Graphics {
	void LoadTexture(
		AssetManager& asset,
		Material& material,
		aiMaterial* aimaterial,
		aiTextureType type,
		const char* name);

	void LoadWeirdTextures(
		AssetManager& asset,
		Material& material,
		aiMaterial* aimaterial);

	void AddNodeToModel(
		aiNode* node,
		std::vector<Mesh>& meshes,
		Model* model);

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
			| aiProcess_GenNormals
			| aiProcess_FlipUVs);

		if (!scene) {
			LOG_WARNING << importer.GetErrorString();
			return nullptr;
		}

		std::vector<iw::ref<Material>> materials;
		if (scene->HasMaterials()) {
			for (size_t i = 0; i < scene->mNumMaterials; i++) {
				aiMaterial* aimaterial = scene->mMaterials[i];
				const char* ainame     = aimaterial->GetName().C_Str();

				ref<iw::Material> loaded = m_asset.Load<iw::Material>(ainame);
				if (loaded) {
					materials.push_back(loaded);
					continue;
				}

				Color diffuse;
				Color ao;
				Color emissiveColor;
				//float metallic;
				//float roughness = 0.2f;
				//float ambientOcclusion;
				
				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_DIFFUSE, (aiColor4D*)&diffuse); 
				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_AMBIENT, (aiColor4D*)&ao);
				aiGetMaterialColor(aimaterial, AI_MATKEY_COLOR_EMISSIVE, (aiColor4D*)&emissiveColor);

				//aiGetMaterialFloat(aimaterial, AI_MATKEY_SHININESS, &metallic);

				//metallic /= 4 * 128; // obj files scale this by 4? and then opengl by 128???

				Material material;

				// Loaded  value
				// Scanned description
				// User value
				
				material.Set("baseColor", diffuse);
				material.Set("ao", ao.a);
				material.Set("emissive", emissiveColor.rgb().length());

				//material.SetFloats("emissiveColor", &emissiveColor, 4);
				//material.SetFloat("metallic", metallic);
				//material.SetFloat("roughness", roughness);

				//if (aimaterial->GetTextureCount(aiTextureType_UNKNOWN) > 0) {
					// this means we are pbr kinda
				//	LoadWeirdTextures(m_asset, material, aimaterial);
				//}

				//else {
					LoadTexture(m_asset, material, aimaterial, aiTextureType_DIFFUSE, "diffuseMap");
				//}

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

		std::vector<Mesh> meshes;

		for (size_t i = 0; i < scene->mNumMeshes; i++) {
			const aiMesh* aimesh = scene->mMeshes[i];
			const char* ainame = aimesh->mName.C_Str();

			bool cont = false;

			for (auto& model : m_loaded) {
				for (const iw::Mesh& mesh : model.second->GetMeshes()) {
					if (mesh.Data()->Name() == ainame) {
						meshes.push_back(mesh);

						cont = true;
						break;
					}
				}

				if (cont) {
					break;
				}
			}

			if (cont) {
				continue;
			}

			MeshDescription description;

			if (aimesh->HasPositions()) {
				description.DescribeBuffer(bName::POSITION, MakeLayout<float>(3));
			}

			if (aimesh->HasNormals()) {
				description.DescribeBuffer(bName::NORMAL, MakeLayout<float>(3));
			}

			if (aimesh->HasTangentsAndBitangents()) {
				description.DescribeBuffer(bName::TANGENT, MakeLayout<float>(3));
				description.DescribeBuffer(bName::BITANGENT, MakeLayout<float>(3));
			}

			// max of 8
			for (unsigned i = 0; i < aimesh->GetNumUVChannels(); i++) {
				bName channel = (bName)((unsigned)bName::UV + i);
				description.DescribeBuffer(channel, MakeLayout<float>(aimesh->mNumUVComponents[i]));
			}

			// max of 8
			for (unsigned i = 0; i < aimesh->GetNumColorChannels(); i++) { // todo: move back above uvs
				bName channel = (bName)((unsigned)bName::COLOR + i);
				description.DescribeBuffer(channel, MakeLayout<float>(4));
			}

			MeshData* data = new MeshData(description);

			if (description.HasBuffer(bName::POSITION)) {
				data->SetBufferData(bName::POSITION, aimesh->mNumVertices, aimesh->mVertices);
			}

			if (description.HasBuffer(bName::NORMAL)) {
				data->SetBufferData(bName::NORMAL, aimesh->mNumVertices, aimesh->mNormals);
			}

			if (   description.HasBuffer(bName::TANGENT)
				&& description.HasBuffer(bName::BITANGENT))
			{
				data->SetBufferData(bName::TANGENT, aimesh->mNumVertices, aimesh->mTangents);
				data->SetBufferData(bName::BITANGENT, aimesh->mNumVertices, aimesh->mBitangents);
			}

			for (int c = 0; c < aimesh->GetNumColorChannels(); c++) {
				bName channel = (bName)((int)bName::COLOR + c);
				if (description.HasBuffer(channel)) {
					data->SetBufferData(channel, aimesh->mNumVertices, aimesh->mColors[c]);
				}
			}

			for (unsigned c = 0; c < aimesh->GetNumUVChannels(); c++) {
				bName channel = (bName)((unsigned)bName::UV + c);
				if (description.HasBuffer(channel)) {
					unsigned uvComponents = aimesh->mNumUVComponents[c];
					unsigned count        = aimesh->mNumVertices * uvComponents;

					float* buffer = new float[count];

					for (int i = 0, j = 0; j < count; i++) {
						switch (uvComponents) {
							case 1: {
								buffer[j++] = aimesh->mTextureCoords[c][i].x;
								break;
							}
							case 2: {
								buffer[j++] = aimesh->mTextureCoords[c][i].x;
								buffer[j++] = aimesh->mTextureCoords[c][i].y;
								break;
							}
							case 3: {
								buffer[j++] = aimesh->mTextureCoords[c][i].x;
								buffer[j++] = aimesh->mTextureCoords[c][i].y;
								buffer[j++] = aimesh->mTextureCoords[c][i].z;
								break;
							}
						}
					}

					data->SetBufferData(channel, aimesh->mNumVertices, buffer);
					
					delete[] buffer;
				}
			}

			unsigned indexCount = 0;
			for (unsigned t = 0; t < aimesh->mNumFaces; t++) {
				indexCount += aimesh->mFaces[t].mNumIndices; // could a face have a diffrent # of incices???
			}

			unsigned* indices = new unsigned int[indexCount];

			for (unsigned f = 0, i = 0; f < aimesh->mNumFaces; f++) {
				const aiFace& face = aimesh->mFaces[f];
				for (size_t v = 0; v < face.mNumIndices; v++) {
					indices[i++] = face.mIndices[v];
				}
			}

			data->SetIndexData(indexCount, indices);
			delete[] indices;

			data->SetName(aimesh->mName.C_Str());

			Mesh mesh = data->MakeInstance();
			mesh.SetMaterial(materials.at(aimesh->mMaterialIndex));

			meshes.push_back(mesh);
						
			//if (aimesh->HasBones()) {
			//	aiBone* aibone = aimesh->mBones[i];
			//	
			//	aiVector3D pos;
			//	aiVector3D rot;
			//	aiVector3D scale;
			//
			//	aibone->mOffsetMatrix.Decompose(scale, rot, pos);
			//
			//	Bone bone;
			//	bone.Offset.Position = iw::vector3(pos.x,     pos.y,   pos.z);
			//	bone.Offset.Scale    = iw::vector3(scale.x, scale.y, scale.z);
			//	bone.Offset.Rotation = iw::quaternion::from_euler_angles(rot.x, rot.y, rot.z);
			//
			//	bone.WeightCount = aibone->mNumWeights;
			//	bone.Weights     = new VertexWeight[bone.WeightCount];
			//
			//	for (size_t i = 0; i < bone.WeightCount; i++) {
			//		aiVertexWeight aiweight = aibone->mWeights[i];
			//
			//		bone.Weights[i].Index  = aiweight.mVertexId;
			//		bone.Weights[i].Weight = aiweight.mWeight;
			//	}
			//}
		}

		Model* model = new Model();

		AddNodeToModel(scene->mRootNode, meshes, model);

		importer.FreeScene();

		return model;
	}

	void AddNodeToModel(
		aiNode* node,
		std::vector<Mesh>& meshes,
		Model* model)
	{
		if (node->mNumMeshes > 0) {
			matrix4 transformation(
				node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4,
				node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4,
				node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4,
				node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4);

			transformation.transpose();

			Transform transform = Transform::FromMatrix(transformation);

			for (unsigned i = 0; i < node->mNumMeshes; i++) {
				unsigned mesh = node->mMeshes[0];

				model->AddMesh(meshes.at(mesh), transform);
			}
		}

		for (unsigned i = 0; i < node->mNumChildren; i++) {
			AddNodeToModel(node->mChildren[i], meshes, model);
		}
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

		//std::vector<aiString> names;

		for (unsigned i = 0; i < count; i++) {
			if (aimaterial->GetTexture(type, i, &texturePath) == AI_SUCCESS) {
				//if (std::find(names.begin(), names.end(), texturePath) != names.end()) {
				//	continue;
				//}

				//names.push_back(texturePath);
				
				std::stringstream ss;
				
				ss << name;
				
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

	void LoadWeirdTextures(
		AssetManager& asset,
		Material& material, 
		aiMaterial* aimaterial)
	{
		aiString texturePath;
		if (aimaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLICROUGHNESS_TEXTURE, &texturePath) == AI_SUCCESS) {
			iw::ref<Texture> texture = asset.Load<Texture>(texturePath.C_Str());
			if (texture) {
				material.SetTexture("roughnessMap", texture);
			}
		}

		if (aimaterial->GetTexture(AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_TEXTURE, &texturePath) == AI_SUCCESS) {
			iw::ref<Texture> texture = asset.Load<Texture>(texturePath.C_Str());
			if (texture) {
				material.SetTexture("albedoMap", texture);
			}
		}
	}
}
}
