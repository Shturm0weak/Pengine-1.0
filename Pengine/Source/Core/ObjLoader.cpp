#include "ObjLoader.h"

#include "../Events/OnMainThreadCallback.h"
#include "../EventSystem/EventSystem.h"

Pengine::Mesh* objl::Loader::Load(const std::string& filePath)
{
	objl::Loader loader;
	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			
			std::vector<float> vertexAttributes;
			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				vertexAttributes.push_back(curMesh.Vertices[j].Position.X);
				vertexAttributes.push_back(-curMesh.Vertices[j].Position.Z);
				vertexAttributes.push_back(curMesh.Vertices[j].Position.Y);

				vertexAttributes.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Z);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Y);

				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.Y);

				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(1.0f);
			}

			std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

			return Pengine::MeshManager::GetInstance().Create(curMesh.MeshName, vertexAttributes, curMesh.Indices, layouts, filePath);
		}
	}
}

void objl::Loader::LoadAsync(const std::string& filePath, std::function<void(Pengine::Mesh*)> callback)
{
	objl::Loader loader;
	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];

			std::vector<float> vertexAttributes;
			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				vertexAttributes.push_back(curMesh.Vertices[j].Position.X);
				vertexAttributes.push_back(-curMesh.Vertices[j].Position.Z);
				vertexAttributes.push_back(curMesh.Vertices[j].Position.Y);

				vertexAttributes.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Z);
				vertexAttributes.push_back(curMesh.Vertices[j].Normal.Y);

				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttributes.push_back(curMesh.Vertices[j].TextureCoordinate.Y);

				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(0.7f);
				vertexAttributes.push_back(1.0f);
			}

			std::vector<uint32_t> layouts = { 3, 3, 2, 4 };

			auto callbackMainThread = [=]() {
				std::vector<float> vertexAttributesTemp = vertexAttributes;
				std::vector<unsigned int> indicesTemp = curMesh.Indices;

				Pengine::MeshManager::GetInstance().CreateAsync(curMesh.MeshName, vertexAttributesTemp, indicesTemp, layouts, callback, filePath);
			};
			Pengine::EventSystem::GetInstance().SendEvent(new Pengine::OnMainThreadCallback(callbackMainThread, Pengine::EventType::ONMAINTHREADPROCESS));
		}
	}
}
