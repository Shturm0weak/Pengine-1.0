#include "MeshLoader.h"

#include "Serializer.h"
#include "Editor.h"
#include "../Components/Renderer3D.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

#include <unordered_map>

using namespace Pengine;

Mesh* MeshLoader::Load(const std::string& filePath)
{
	Mesh* meshToReturn = nullptr;
	Mesh::Meta meta = Serializer::DeserializeMeshMeta(filePath);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(meta.m_FilePath, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error(std::string("ASSIMP::" + std::string(import.GetErrorString())).c_str());
		return meshToReturn;
	}

	std::unordered_map<std::string, size_t> names;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		 aiMesh* aiMesh = scene->mMeshes[i];

		std::string meshName = aiMesh->mName.C_Str();
		if (names.count(meshName) > 0)
		{
			meshName += "_" + std::to_string(i);
		}
		else
		{
			names[meshName] = i;
		}

		std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByName;
		std::vector<float> vertexAttributes;
		std::vector<uint32_t> indices;
		glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
		glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		LoadVertexData(aiMesh, vertexAttributes, indices, boneInfoByName, minBoundingBox, maxBoundingBox);

		std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3, 4, 4 };

		Mesh* mesh = MeshManager::GetInstance().Create(meshName, vertexAttributes, indices, layouts, filePath);
		mesh->m_BoundingBox = { minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
		mesh->m_BoneInfoByName = boneInfoByName;

		if (meshName == meta.m_Name)
		{
			mesh->m_CullFace = meta.m_CullFace;
			mesh->m_ShadowCullFace = meta.m_ShadowCullFace;
			meshToReturn = mesh;
			return meshToReturn;
		}
	}

	return meshToReturn;
}

void MeshLoader::LoadAsync(const std::string& filePath, std::function<void(Mesh*)> callback)
{
	Mesh::Meta meta = Serializer::DeserializeMeshMeta(filePath);

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(meta.m_FilePath, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error(std::string("ASSIMP::" + std::string(import.GetErrorString())).c_str());
		return;
	}

	std::unordered_map<std::string, size_t> names;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = scene->mMeshes[i];

		std::string meshName = aiMesh->mName.C_Str();
		if (names.count(meshName) > 0)
		{
			meshName += "_" + std::to_string(i);
		}
		else
		{
			names[meshName] = i;
		}

		std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByName;
		std::vector<float> vertexAttributes;
		std::vector<uint32_t> indices;
		glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
		glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		LoadVertexData(aiMesh, vertexAttributes, indices, boneInfoByName, minBoundingBox, maxBoundingBox);

		if (meshName == meta.m_Name)
		{
			auto callbackMainThread = [=]() {
				std::vector<float> vertexAttributesTemp = vertexAttributes;
				std::vector<unsigned int> indicesTemp = indices;
				std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByNameTemp = boneInfoByName;
				std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3, 4, 4 };

				MeshManager::GetInstance().CreateAsync(meshName, vertexAttributesTemp, indicesTemp, layouts, callback, filePath);
				Mesh* mesh = MeshManager::GetInstance().Get(filePath);
				mesh->m_BoundingBox =
				{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
				mesh->m_CullFace = meta.m_CullFace;
				mesh->m_ShadowCullFace = meta.m_ShadowCullFace;
				mesh->m_BoneInfoByName = boneInfoByNameTemp;
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callbackMainThread, EventType::ONMAINTHREADPROCESS));
		}
		else
		{
			auto callbackMainThread = [=]() {
				std::vector<float> vertexAttributesTemp = vertexAttributes;
				std::vector<unsigned int> indicesTemp = indices;
				std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByNameTemp = boneInfoByName;
				std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3, 4, 4 };

				std::string metaFilePath = Serializer::GenerateMetaFilePath(meta.m_FilePath, meshName);
				Mesh::Meta meta = Serializer::DeserializeMeshMeta(metaFilePath);

				MeshManager::GetInstance().Create(meshName, vertexAttributesTemp, indicesTemp, layouts, metaFilePath);
				Mesh* mesh = MeshManager::GetInstance().Get(metaFilePath);
				mesh->m_BoundingBox =
				{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
				mesh->m_CullFace = meta.m_CullFace;
				mesh->m_ShadowCullFace = meta.m_ShadowCullFace;
				mesh->m_BoneInfoByName = boneInfoByNameTemp;
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callbackMainThread, EventType::ONMAINTHREADPROCESS));
		}
	}
}

void MeshLoader::LoadAsyncToViewport(const std::string& filePath)
{
	MeshLoader loader;

	std::vector<std::pair<std::string, std::string>> meshesFilePath;

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filePath, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error(std::string("ASSIMP::" + std::string(import.GetErrorString())).c_str());
		return;
	}

	std::unordered_map<std::string, size_t> names;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = scene->mMeshes[i];

		std::string meshName = aiMesh->mName.C_Str();
		if (names.count(meshName) > 0)
		{
			meshName += "_" + std::to_string(i);
		}
		else
		{
			names[meshName] = i;
		}

		Mesh::Meta meta;
		meta.m_Name = meshName;
		meta.m_FilePath = filePath;

		meshesFilePath.emplace_back(meta.m_Name, Serializer::SerializeMeshMeta(meta));

		std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByName;
		std::vector<float> vertexAttributes;
		std::vector<uint32_t> indices;
		glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
		glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

		LoadVertexData(aiMesh, vertexAttributes, indices, boneInfoByName, minBoundingBox, maxBoundingBox);

		std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3, 4, 4 };

		auto callbackMainThread = [=]() {
			std::vector<float> vertexAttributesTemp = vertexAttributes;
			std::vector<unsigned int> indicesTemp = indices;
			std::unordered_map<std::string, Mesh::BoneInfo> boneInfoByNameTemp = boneInfoByName;

			Mesh* mesh = MeshManager::GetInstance().Create(meshesFilePath.back().first, vertexAttributesTemp, indicesTemp, layouts, meshesFilePath.back().second);
			MeshManager::GetInstance().Get(meshesFilePath.back().second)->m_BoundingBox =
			{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
			mesh->m_BoneInfoByName = boneInfoByNameTemp;
		};
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callbackMainThread, EventType::ONMAINTHREADPROCESS));
	}

	auto callbackMainThread = [meshesFilePath, filePath]() {
		Scene& scene = Editor::GetInstance().GetCurrentScene();

		GameObject* owner = nullptr;
		if (meshesFilePath.size() > 1)
		{
			owner = scene.CreateGameObject(Utils::GetNameFromFilePath(filePath));
		}

		for (auto meta : meshesFilePath)
		{
			GameObject* gameObject = scene.CreateGameObject(meta.first);
			Renderer3D* r3d = gameObject->m_ComponentManager.AddComponent<Renderer3D>();
			MeshManager::GetInstance().GetAsync(meta.second,
				[r3d](Mesh* mesh)
				{
					r3d->SetMesh(mesh);
				});

			if (owner)
			{
				owner->AddChild(gameObject);
			}
		}
	};
	EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callbackMainThread, EventType::ONMAINTHREADPROCESS));
}

void MeshLoader::GenerateMeshMeta(const std::string& filePath, bool onlyMissing)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filePath, aiProcess_Triangulate);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		Logger::Error(std::string("ASSIMP::" + std::string(import.GetErrorString())).c_str());
		return;
	}

	std::unordered_map<std::string, size_t> names;
	for (uint32_t i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = scene->mMeshes[i];

		std::string meshName = aiMesh->mName.C_Str();
		if (names.count(meshName) > 0)
		{
			meshName += "_" + std::to_string(i);
		}
		else
		{
			names[meshName] = i;
		}

		Mesh::Meta newMeta;
		newMeta.m_Name = meshName;
		newMeta.m_FilePath = filePath;
		newMeta.m_CullFace = true;
		newMeta.m_ShadowCullFace = GL_FRONT;

		if (onlyMissing)
		{
			Mesh::Meta meta = Serializer::DeserializeMeshMeta(Serializer::GenerateMetaFilePath(filePath, meshName));
			if (meta.m_FilePath.empty())
			{
				Serializer::SerializeMeshMeta(newMeta);
			}
		}
		else
		{
			Serializer::SerializeMeshMeta(newMeta);
		}
	}
}

void MeshLoader::LoadVertexData(aiMesh* aiMesh, std::vector<float>& vertexAttributes,
	std::vector<uint32_t>& indices, std::unordered_map<std::string, Mesh::BoneInfo>& boneInfoByName,
	glm::vec3& minBoundingBox, glm::vec3& maxBoundingBox)
{
	const size_t uvChannel = 0;

	for (uint32_t j = 0; j < aiMesh->mNumVertices; ++j)
	{
		glm::vec3 position =
		{
			aiMesh->mVertices[j].x,
			aiMesh->mVertices[j].y,
			aiMesh->mVertices[j].z
		};

		// Position.
		vertexAttributes.push_back(position.x);
		vertexAttributes.push_back(position.y);
		vertexAttributes.push_back(position.z);

		// Normal.
		vertexAttributes.push_back(aiMesh->mNormals[j].x);
		vertexAttributes.push_back(aiMesh->mNormals[j].y);
		vertexAttributes.push_back(aiMesh->mNormals[j].z);

		// UV.
		vertexAttributes.push_back(aiMesh->mTextureCoords[uvChannel][j].x);
		vertexAttributes.push_back(aiMesh->mTextureCoords[uvChannel][j].y);

		// Tangent.
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);

		// Bitangent.
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);

		// Weights.
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);

		// BonesIds.
		vertexAttributes.push_back(-1.0f);
		vertexAttributes.push_back(-1.0f);
		vertexAttributes.push_back(-1.0f);
		vertexAttributes.push_back(-1.0f);

		if (maxBoundingBox.x < position.x)
		{
			maxBoundingBox.x = position.x;
		}
		else if (minBoundingBox.x > position.x)
		{
			minBoundingBox.x = position.x;
		}

		if (maxBoundingBox.y < position.y)
		{
			maxBoundingBox.y = position.y;
		}
		else if (minBoundingBox.y > position.y)
		{
			minBoundingBox.y = position.y;
		}

		if (maxBoundingBox.z < position.z)
		{
			maxBoundingBox.z = position.z;
		}
		else if (minBoundingBox.z > position.z)
		{
			minBoundingBox.z = position.z;
		}
	}

	const size_t vertexOffset = Mesh::GetVertexOffset();
	for (uint32_t j = 0; j < aiMesh->mNumFaces; ++j)
	{
		for (uint32_t i = 0; i < aiMesh->mFaces[j].mNumIndices; ++i)
		{
			indices.emplace_back(aiMesh->mFaces[j].mIndices[i]);
		}

		size_t i0 = aiMesh->mFaces[j].mIndices[0];
		size_t i1 = aiMesh->mFaces[j].mIndices[1];
		size_t i2 = aiMesh->mFaces[j].mIndices[2];

		glm::vec3 pos1 = { aiMesh->mVertices[i0].x, aiMesh->mVertices[i0].y, aiMesh->mVertices[i0].z };
		glm::vec3 pos2 = { aiMesh->mVertices[i1].x, aiMesh->mVertices[i1].y, aiMesh->mVertices[i1].z };
		glm::vec3 pos3 = { aiMesh->mVertices[i2].x, aiMesh->mVertices[i2].y, aiMesh->mVertices[i2].z };

		glm::vec2 uv1 = { aiMesh->mTextureCoords[uvChannel][i0].x, aiMesh->mTextureCoords[uvChannel][i0].y };
		glm::vec2 uv2 = { aiMesh->mTextureCoords[uvChannel][i1].x, aiMesh->mTextureCoords[uvChannel][i1].y };
		glm::vec2 uv3 = { aiMesh->mTextureCoords[uvChannel][i2].x, aiMesh->mTextureCoords[uvChannel][i2].y };

		glm::vec3 edge1 = pos2 - pos1;
		glm::vec3 edge2 = pos3 - pos1;
		glm::vec2 deltaUV1 = uv2 - uv1;
		glm::vec2 deltaUV2 = uv3 - uv1;

		float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		glm::vec3 tangent, bitangent;

		tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
		tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
		tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
		tangent = glm::normalize(tangent);

		bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
		bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
		bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
		bitangent = glm::normalize(bitangent);

		vertexAttributes[i0 * vertexOffset + 8] = tangent.x;
		vertexAttributes[i0 * vertexOffset + 9] = tangent.y;
		vertexAttributes[i0 * vertexOffset + 10] = tangent.z;
		vertexAttributes[i0 * vertexOffset + 11] = bitangent.x;
		vertexAttributes[i0 * vertexOffset + 12] = bitangent.y;
		vertexAttributes[i0 * vertexOffset + 13] = bitangent.z;

		vertexAttributes[i1 * vertexOffset + 8] = tangent.x;
		vertexAttributes[i1 * vertexOffset + 9] = tangent.y;
		vertexAttributes[i1 * vertexOffset + 10] = tangent.z;
		vertexAttributes[i1 * vertexOffset + 11] = bitangent.x;
		vertexAttributes[i1 * vertexOffset + 12] = bitangent.y;
		vertexAttributes[i1 * vertexOffset + 13] = bitangent.z;

		vertexAttributes[i2 * vertexOffset + 8] = tangent.x;
		vertexAttributes[i2 * vertexOffset + 9] = tangent.y;
		vertexAttributes[i2 * vertexOffset + 10] = tangent.z;
		vertexAttributes[i2 * vertexOffset + 11] = bitangent.x;
		vertexAttributes[i2 * vertexOffset + 12] = bitangent.y;
		vertexAttributes[i2 * vertexOffset + 13] = bitangent.z;
	}

	for (uint32_t i = 0; i < aiMesh->mNumBones; ++i)
	{
		aiBone* bone = aiMesh->mBones[i];
		int boneID = -1;
		std::string boneName = bone->mName.C_Str();
		if (boneInfoByName.find(boneName) == boneInfoByName.end())
		{
			Mesh::BoneInfo newBoneInfo;
			newBoneInfo.id = boneInfoByName.size();
			newBoneInfo.offset = AssimpGLMHelpers::ConvertMatrixToGLMFormat(bone->mOffsetMatrix);
			boneID = newBoneInfo.id;
			boneInfoByName[boneName] = newBoneInfo;
		}
		else
		{
			boneID = boneInfoByName[boneName].id;
		}

		assert(boneID != -1);

		for (int j = 0; j < bone->mNumWeights; ++j)
		{
			aiVertexWeight aiWeight = bone->mWeights[j];
			int vertexId = aiWeight.mVertexId;
			float weight = aiWeight.mWeight;

			for (int k = 0; k < MAX_BONE_WEIGHTS; ++k)
			{
				if (vertexAttributes[vertexId * vertexOffset + 18 + k] < 0)
				{
					vertexAttributes[vertexId * vertexOffset + 14 + k] = weight;
					vertexAttributes[vertexId * vertexOffset + 18 + k] = boneID;
					break;
				}
			}
		}
	}
}
