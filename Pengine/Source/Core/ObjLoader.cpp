#include "ObjLoader.h"

Pengine::Mesh* objl::Loader::Load(const std::string& filePath)
{
	objl::Loader loader;
	bool loadOut = loader.LoadFile(filePath);
	if (loadOut)
	{
		for (size_t i = 0; i < loader.LoadedMeshes.size(); i++)
		{
			objl::Mesh curMesh = loader.LoadedMeshes[i];
			
			std::vector<float> vertexAttribute;
			for (size_t j = 0; j < curMesh.Vertices.size(); j++)
			{
				vertexAttribute.push_back(curMesh.Vertices[j].Position.X);
				vertexAttribute.push_back(curMesh.Vertices[j].Position.Y);
				vertexAttribute.push_back(curMesh.Vertices[j].Position.Z);

				vertexAttribute.push_back(curMesh.Vertices[j].Normal.X);
				vertexAttribute.push_back(curMesh.Vertices[j].Normal.Y);
				vertexAttribute.push_back(curMesh.Vertices[j].Normal.Z);

				vertexAttribute.push_back(curMesh.Vertices[j].TextureCoordinate.X);
				vertexAttribute.push_back(curMesh.Vertices[j].TextureCoordinate.Y);
			}

			std::vector<uint32_t> layouts = { 3, 3, 2 };

			return Pengine::MeshManager::GetInstance().Create(curMesh.MeshName, vertexAttribute, curMesh.Indices, layouts, filePath);
		}
	}
}