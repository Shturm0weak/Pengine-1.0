#include "ObjLoader.h"

#include "Serializer.h"
#include "Editor.h"
#include "../Components/Renderer3D.h"
#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

Pengine::Mesh* objl::Loader::Load(const std::string& filePath)
{
	Pengine::Mesh* meshToReturn = nullptr;
	Pengine::Mesh::Meta meta = Pengine::Serializer::DeserializeMeshMeta(filePath);
	
	objl::Loader loader;

	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			
			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				glm::vec3 position = 
				{ 
					curMesh.Vertices[j].Position.X,
					curMesh.Vertices[j].Position.Y,
					curMesh.Vertices[j].Position.Z
				};

				vertexAttributes.push_back(position.x);
				vertexAttributes.push_back(position.y);
				vertexAttributes.push_back(position.z);

				vertexAttributes.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Y);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Z);

				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.Y);

				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);

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

			std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

			Pengine::Mesh* mesh = Pengine::MeshManager::GetInstance().Create(curMesh.MeshName, vertexAttributes, curMesh.Indices, layouts, filePath);
			mesh->m_BoundingBox = { minBoundingBox, maxBoundingBox };
			
			if (curMesh.MeshName == meta.m_Name)
			{
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
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			
			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				glm::vec3 position =
				{
					curMesh.Vertices[j].Position.X,
					curMesh.Vertices[j].Position.Y,
					curMesh.Vertices[j].Position.Z
				};

				vertexAttributes.push_back(position.x);
				vertexAttributes.push_back(position.y);
				vertexAttributes.push_back(position.z);

				vertexAttributes.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Y);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Z);

				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.Y);

				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);

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

			if (curMesh.MeshName == meta.m_Name)
			{
				auto callbackMainThread = [=]() {
					std::vector<float> vertexAttributesTemp = vertexAttributes;
					std::vector<unsigned int> indicesTemp = curMesh.Indices;
					std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

					Pengine::MeshManager::GetInstance().CreateAsync(curMesh.MeshName, vertexAttributesTemp, indicesTemp, layouts, callback, filePath);
					Pengine::MeshManager::GetInstance().Get(filePath)->m_BoundingBox = { minBoundingBox, maxBoundingBox };
				};
				Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
			}
			else
			{
				auto callbackMainThread = [=]() {
					std::vector<float> vertexAttributesTemp = vertexAttributes;
					std::vector<unsigned int> indicesTemp = curMesh.Indices;
					std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

					std::string metaFilePath = Pengine::Serializer::GenerateMetaFilePath(meta.m_FilePath, curMesh.MeshName);

					Pengine::MeshManager::GetInstance().Create(curMesh.MeshName, vertexAttributesTemp, indicesTemp, layouts, metaFilePath);
					Pengine::MeshManager::GetInstance().Get(metaFilePath)->m_BoundingBox = { minBoundingBox, maxBoundingBox };
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
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			Pengine::Mesh::Meta meta;
			meta.m_Name = curMesh.MeshName;
			meta.m_FilePath = filePath;

			meshesFilePath.emplace_back(meta.m_Name, Pengine::Serializer::SerializeMeshMeta(filePath, meta));

			std::vector<float> vertexAttributes;
			glm::vec3 minBoundingBox = { FLT_MAX, FLT_MAX, FLT_MAX };
			glm::vec3 maxBoundingBox = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				glm::vec3 position =
				{
					curMesh.Vertices[j].Position.X,
					curMesh.Vertices[j].Position.Y,
					curMesh.Vertices[j].Position.Z
				};

				vertexAttributes.push_back(position.x);
				vertexAttributes.push_back(position.y);
				vertexAttributes.push_back(position.z);

				vertexAttributes.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Y);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Z);

				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.Y);

				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);
				vertexAttributes.push_back(1.0f);

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

			std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

			auto callbackMainThread = [=]() {
				std::vector<float> vertexAttributesTemp = vertexAttributes;
				std::vector<unsigned int> indicesTemp = curMesh.Indices;

				Pengine::MeshManager::GetInstance().Create(meshesFilePath.back().first, vertexAttributesTemp, indicesTemp, layouts, meshesFilePath.back().second);
				Pengine::MeshManager::GetInstance().Get(meshesFilePath.back().second)->m_BoundingBox = { minBoundingBox, maxBoundingBox };
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

void objl::Loader::GenerateMeshMeta(const std::string& filePath)
{
	objl::Loader loader;

	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			Pengine::Mesh::Meta meta;
			meta.m_Name = curMesh.MeshName;
			meta.m_FilePath = filePath;

			Pengine::Serializer::SerializeMeshMeta(filePath, meta);
		}
	}
}
