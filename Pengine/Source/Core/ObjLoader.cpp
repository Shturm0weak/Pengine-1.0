#include "ObjLoader.h"

#include "Serializer.h"
#include "Editor.h"
#include "../Components/Renderer3D.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

#include <unordered_map>

Pengine::Mesh* objl::Loader::Load(const std::string& filePath)
{
	Pengine::Mesh* meshToReturn = nullptr;
	Pengine::Mesh::Meta meta = Pengine::Serializer::DeserializeMeshMeta(filePath);

	objl::Loader loader;

	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		std::unordered_map<std::string, size_t> names;
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			std::string meshName = curMesh.MeshName;
			if (names.count(meshName) > 0)
			{
				meshName += "_" + std::to_string(i);
			}
			else
			{
				names[meshName] = i;
			}

			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			LoadVertexData(curMesh.Vertices, curMesh.Indices, vertexAttributes, minBoundingBox, maxBoundingBox);

			std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3 };

			Pengine::Mesh* mesh = Pengine::MeshManager::GetInstance().Create(meshName, vertexAttributes, curMesh.Indices, layouts, filePath);
			mesh->m_BoundingBox = { minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };

			if (meshName == meta.m_Name)
			{
				mesh->m_CullFace = meta.m_CullFace;
				meshToReturn = mesh;
			}

			return meshToReturn;
		}
	}
}

void objl::Loader::LoadAsync(const std::string& filePath, std::function<void(Pengine::Mesh*)> callback)
{
	Pengine::Mesh::Meta meta = Pengine::Serializer::DeserializeMeshMeta(filePath);

	objl::Loader loader;

	bool loadOut = loader.LoadFile(meta.m_FilePath);
	if (loadOut)
	{
		std::unordered_map<std::string, size_t> names;
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			std::string meshName = curMesh.MeshName;
			if (names.count(meshName) > 0)
			{
				meshName += "_" + std::to_string(i);
			}
			else
			{
				names[meshName] = i;
			}

			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			LoadVertexData(curMesh.Vertices, curMesh.Indices, vertexAttributes, minBoundingBox, maxBoundingBox);

			if (meshName == meta.m_Name)
			{
				auto callbackMainThread = [=]() {
					std::vector<float> vertexAttributesTemp = vertexAttributes;
					std::vector<unsigned int> indicesTemp = curMesh.Indices;
					std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3 };

					Pengine::MeshManager::GetInstance().CreateAsync(meshName, vertexAttributesTemp, indicesTemp, layouts, callback, filePath);
					Pengine::Mesh* mesh = Pengine::MeshManager::GetInstance().Get(filePath);
					mesh->m_BoundingBox =
					{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
					mesh->m_CullFace = meta.m_CullFace;
				};
				Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
			}
			else
			{
				auto callbackMainThread = [=]() {
					std::vector<float> vertexAttributesTemp = vertexAttributes;
					std::vector<unsigned int> indicesTemp = curMesh.Indices;
					std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3 };

					std::string metaFilePath = Pengine::Serializer::GenerateMetaFilePath(meta.m_FilePath, meshName);
					Pengine::Mesh::Meta meta = Pengine::Serializer::DeserializeMeshMeta(metaFilePath);

					Pengine::MeshManager::GetInstance().Create(meshName, vertexAttributesTemp, indicesTemp, layouts, metaFilePath);
					Pengine::Mesh* mesh = Pengine::MeshManager::GetInstance().Get(metaFilePath);
					mesh->m_BoundingBox =
					{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
					mesh->m_CullFace = meta.m_CullFace;
				};
				Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
			}
		}
	}
}

void objl::Loader::LoadAsyncToViewport(const std::string& filePath)
{
	objl::Loader loader;

	std::vector<std::pair<std::string, std::string>> meshesFilePath;

	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		std::unordered_map<std::string, size_t> names;
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			std::string meshName = curMesh.MeshName;
			if (names.count(meshName) > 0)
			{
				meshName += "_" + std::to_string(i);
			}
			else
			{
				names[meshName] = i;
			}

			Pengine::Mesh::Meta meta;
			meta.m_Name = meshName;
			meta.m_FilePath = filePath;

			meshesFilePath.emplace_back(meta.m_Name, Pengine::Serializer::SerializeMeshMeta(meta));

			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			LoadVertexData(curMesh.Vertices, curMesh.Indices, vertexAttributes, minBoundingBox, maxBoundingBox);

			std::vector<uint32_t> layouts = { 3, 3, 2, 3, 3 };

			auto callbackMainThread = [=]() {
				std::vector<float> vertexAttributesTemp = vertexAttributes;
				std::vector<unsigned int> indicesTemp = curMesh.Indices;

				Pengine::MeshManager::GetInstance().Create(meshesFilePath.back().first, vertexAttributesTemp, indicesTemp, layouts, meshesFilePath.back().second);
				Pengine::MeshManager::GetInstance().Get(meshesFilePath.back().second)->m_BoundingBox =
				{ minBoundingBox, maxBoundingBox, (minBoundingBox + maxBoundingBox) * 0.5f };
			};
			Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
		}

		auto callbackMainThread = [meshesFilePath, filePath]() {
			Pengine::Scene& scene = Pengine::Editor::GetInstance().GetCurrentScene();

			Pengine::GameObject* owner = nullptr;
			if (meshesFilePath.size() > 1)
			{
				owner = scene.CreateGameObject(Pengine::Utils::GetNameFromFilePath(filePath));
			}

			for (auto meta : meshesFilePath)
			{
				Pengine::GameObject* gameObject = scene.CreateGameObject(meta.first);
				Pengine::Renderer3D* r3d = gameObject->m_ComponentManager.AddComponent<Pengine::Renderer3D>();
				Pengine::MeshManager::GetInstance().GetAsync(meta.second,
					[r3d](Pengine::Mesh* mesh)
					{
						r3d->SetMesh(mesh);
					});

				if (owner)
				{
					owner->AddChild(gameObject);
				}
			}
		};
		Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
	}
}

void objl::Loader::GenerateMeshMeta(const std::string& filePath, bool onlyMissing)
{
	objl::Loader loader;

	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		std::unordered_map<std::string, size_t> names;
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			std::string meshName = curMesh.MeshName;
			if (names.count(meshName) > 0)
			{
				meshName += "_" + std::to_string(i);
			}
			else
			{
				names[meshName] = i;
			}

			Pengine::Mesh::Meta newMeta;
			newMeta.m_Name = meshName;
			newMeta.m_FilePath = filePath;
			newMeta.m_CullFace = true;

			if (onlyMissing)
			{
				Pengine::Mesh::Meta meta = Pengine::Serializer::DeserializeMeshMeta(Pengine::Serializer::GenerateMetaFilePath(filePath, meshName));
				if (meta.m_FilePath.empty())
				{
					Pengine::Serializer::SerializeMeshMeta(newMeta);
				}
			}
			else
			{
				Pengine::Serializer::SerializeMeshMeta(newMeta);
			}
		}
	}
}

void objl::Loader::LoadVertexData(const std::vector<objl::Vertex>& vertices,
	const std::vector<uint32_t>& indices, std::vector<float>& vertexAttributes,
	glm::vec3& minBoundingBox, glm::vec3& maxBoundingBox)
{
	const size_t verticesSize = vertices.size();
	for (size_t j = 0; j < verticesSize; j++)
	{
		glm::vec3 position =
		{
			vertices[j].Position.X,
			vertices[j].Position.Y,
			vertices[j].Position.Z
		};

		// Position.
		vertexAttributes.push_back(position.x);
		vertexAttributes.push_back(position.y);
		vertexAttributes.push_back(position.z);

		// Normal.
		vertexAttributes.push_back(vertices[j].Normal.X);
		vertexAttributes.push_back(vertices[j].Normal.Y);
		vertexAttributes.push_back(vertices[j].Normal.Z);

		// UV.
		vertexAttributes.push_back(vertices[j].TextureCoordinate.X);
		vertexAttributes.push_back(vertices[j].TextureCoordinate.Y);

		// Tangent.
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);

		// Bitangent.
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);
		vertexAttributes.push_back(0.0f);

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

	const size_t vertexOffset = Pengine::Mesh::GetVertexOffset();
	const size_t indicesSize = indices.size();
	for (size_t j = 0; j < indicesSize; j += 3)
	{
		size_t i0 = indices[j + 0];
		size_t i1 = indices[j + 1];
		size_t i2 = indices[j + 2];

		glm::vec3 pos1 = { vertices[i0].Position.X, vertices[i0].Position.Y, vertices[i0].Position.Z };
		glm::vec3 pos2 = { vertices[i1].Position.X, vertices[i1].Position.Y, vertices[i1].Position.Z };
		glm::vec3 pos3 = { vertices[i2].Position.X, vertices[i2].Position.Y, vertices[i2].Position.Z };

		glm::vec2 uv1 = { vertices[i0].TextureCoordinate.X, vertices[i0].TextureCoordinate.Y };
		glm::vec2 uv2 = { vertices[i1].TextureCoordinate.X, vertices[i1].TextureCoordinate.Y };
		glm::vec2 uv3 = { vertices[i2].TextureCoordinate.X, vertices[i2].TextureCoordinate.Y };

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
}
