#pragma once

#include "Core.h"
#include "Mesh.h"

#include <unordered_map>
#include <map>
#include <functional>

namespace Pengine
{

	class PENGINE_API MeshManager
	{
	private:

		std::unordered_map<std::string, Mesh*> m_Meshes;
		std::multimap<std::string, std::function<void(Mesh*)>> m_WaitingForMeshes;
		std::vector<std::string> m_MeshesIsLoading;

		void DispatchLoadedMeshes();

		MeshManager() = default;
		MeshManager(const MeshManager&) = delete;
		MeshManager& operator=(const MeshManager&) { return *this; }
		~MeshManager() = default;

		friend class Editor;
	public:

		static MeshManager& GetInstance();

		Mesh* Load(const std::string& filePath);
		
		void LoadAsync(const std::string& filePath, std::function<void(Mesh*)> callback);

		void GenerateMeshMeta(const std::string& filePath);
		
		void LoadAsyncToViewport(const std::string& filePath);

		Mesh* Create(const std::string& name, std::vector<float>& vertexAttributes, 
			std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath = "None");

		void CreateAsync(const std::string& name, std::vector<float>& vertexAttributes,
			std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, std::function<void(Mesh*)> callback, const std::string& filePath = "None");

		Mesh* Get(const std::string& filePath) const;

		void GetAsync(const std::string& filePath, std::function<void(Mesh*)> callback);

		void Delete(Mesh* mesh);

		void ShutDown();
	};

}
