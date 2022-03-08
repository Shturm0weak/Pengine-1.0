#include "MeshManager.h"

#include "Logger.h"
#include "ObjLoader.h"

using namespace Pengine;

MeshManager& MeshManager::GetInstance()
{
	static MeshManager meshManager;
	return meshManager;
}

Mesh* MeshManager::Load(const std::string& filePath)
{
	return objl::Loader::Load(filePath);
}

Mesh* MeshManager::Create(const std::string& name, std::vector<float>& vertexAttributes,
	std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath)
{
	Mesh* mesh = new Mesh(name, vertexAttributes, indices, layouts, filePath);
	m_Meshes.insert(std::make_pair(mesh->GetName(), mesh));
	return mesh;
}

Mesh* MeshManager::Get(const std::string& name) const
{
	auto meshIter = m_Meshes.find(name);
	if (meshIter != m_Meshes.end())
	{
		return meshIter->second;
	}
	else
	{

#ifdef SHOW_MESH_WARNINGS
		Logger::Warning("has not been found!", "Mesh", name.c_str());
#endif

		return nullptr;
	}
}

void MeshManager::ShutDown()
{
	for (auto& meshIter : m_Meshes)
	{
		delete meshIter.second;
	}
	m_Meshes.clear();
}
