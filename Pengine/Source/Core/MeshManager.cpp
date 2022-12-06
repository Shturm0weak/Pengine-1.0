#include "MeshManager.h"

#include "Logger.h"
#include "ObjLoader.h"
#include "ThreadPool.h"
#include "Serializer.h"

using namespace Pengine;

void MeshManager::DispatchLoadedMeshes()
{
	for (auto i = m_WaitingForMeshes.begin(); i != m_WaitingForMeshes.end();)
	{
		std::function<void(Mesh*)> callback = i->second;
		Mesh* mesh = Get(i->first);

		if (!mesh) mesh = Get(i->first);

		if (mesh != nullptr)
		{
			callback(mesh);
			m_WaitingForMeshes.erase(i++);
		}
		else
		{
			++i;
		}
	}
}

MeshManager& MeshManager::GetInstance()
{
	static MeshManager meshManager;
	return meshManager;
}

Mesh* MeshManager::Load(const std::string& filePath)
{
	if (Mesh* mesh = Get(filePath))
	{
		return mesh;
	}
	else
	{
		return objl::Loader::Load(filePath);
	}
}

void MeshManager::LoadAsync(const std::string& filePath, std::function<void(Mesh*)> callback)
{
	if (Mesh* mesh = Get(filePath))
	{
		callback(mesh);
		return;
	}

	Mesh::Meta meta = Serializer::DeserializeMeshMeta(filePath);
	if (meta.m_FilePath.empty())
	{
		Logger::Error("file doesn't exist!", "Mesh::Meta", filePath.c_str());
		return;
	}

	if (Utils::IsThere<std::string>(m_MeshesIsLoading, meta.m_FilePath))
	{
		m_WaitingForMeshes.emplace(filePath, callback);
	}
	else
	{
		m_MeshesIsLoading.push_back(meta.m_FilePath);
		ThreadPool::GetInstance().Enqueue([=] {
			objl::Loader::LoadAsync(filePath, callback);
			Utils::Erase<std::string>(m_MeshesIsLoading, meta.m_FilePath);
		});
	}	
}

void MeshManager::GenerateMeshMeta(const std::string& filePath, bool onlyMissing)
{
	ThreadPool::GetInstance().Enqueue([=] {
		objl::Loader::GenerateMeshMeta(filePath, onlyMissing);
	});
}

void MeshManager::LoadAsyncToViewport(const std::string& filePath)
{
	ThreadPool::GetInstance().Enqueue([=] {
		objl::Loader::LoadAsyncToViewport(filePath);
	});
}

Mesh* MeshManager::Create(const std::string& name, std::vector<float>& vertexAttributes,
	std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath)
{
	if (Mesh* mesh = Get(filePath))
	{
		return mesh;
	}
	else
	{
		mesh = new Mesh(name, vertexAttributes, indices, layouts, filePath);
		m_Meshes.insert(std::make_pair(mesh->GetFilePath(), mesh));
		DispatchLoadedMeshes();

		return mesh;
	}
}

void MeshManager::CreateAsync(const std::string& name, std::vector<float>& vertexAttributes, std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, std::function<void(Mesh*)> callback, const std::string& filePath)
{
	if (Mesh* mesh = Get(filePath))
	{
		callback(mesh);
	}
	else
	{
		mesh = new Mesh(name, vertexAttributes, indices, layouts, filePath);
		m_Meshes.insert(std::make_pair(mesh->GetFilePath(), mesh));
		callback(mesh);

		DispatchLoadedMeshes();
	}
}

Mesh* MeshManager::Get(const std::string& filePath) const
{
	auto meshIter = m_Meshes.find(filePath);
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

void MeshManager::GetAsync(const std::string& filePath, std::function<void(Mesh*)> callback)
{
	if (Mesh* mesh = Get(filePath))
	{
		callback(mesh);
	}
	else
	{
		m_WaitingForMeshes.emplace(filePath, callback);
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
		m_Meshes.erase(mesh->GetFilePath());
		delete mesh;
	}
}