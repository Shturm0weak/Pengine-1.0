#include "MeshManager.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "ThreadPool.h"

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

void MeshManager::LoadAsync(const std::string& filePath, std::function<void(Mesh*)> callback)
{
	ThreadPool::GetInstance().Enqueue([=] {
		objl::Loader::LoadAsync(filePath, callback);
	});
}

Mesh* MeshManager::Create(const std::string& name, std::vector<float>& vertexAttributes,
	std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath)
{
	Mesh* mesh = new Mesh(name, vertexAttributes, indices, layouts, filePath);
	m_Meshes.insert(std::make_pair(mesh->GetName(), mesh));
	return mesh;
}

void MeshManager::CreateAsync(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, std::function<void(Mesh*)> callback, const std::string& filePath)
{
	Mesh* mesh = new Mesh(name, vertexAttributes, indices, layouts, filePath);
	m_Meshes.insert(std::make_pair(mesh->GetName(), mesh));
	callback(mesh);
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

void MeshManager::Delete(Mesh* mesh)
{
	if (mesh)
	{
		m_Meshes.erase(mesh->GetName());
		delete mesh;
	}
}