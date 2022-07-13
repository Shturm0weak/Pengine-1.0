#pragma once

#include "Core.h"
#include "Mesh.h"

#include <unordered_map>
#include <functional>

namespace Pengine
{

	class PENGINE_API MeshManager
	{
	private:

		std::unordered_map<std::string, Mesh*> m_Meshes;

		MeshManager() = default;
		MeshManager(const MeshManager&) = delete;
		MeshManager& operator=(const MeshManager&) { return *this; }
		~MeshManager() = default;
	public:

		static MeshManager& GetInstance();

		Mesh* Load(const std::string& filePath);
		
		void LoadAsync(const std::string& filePath, std::function<void(Mesh*)> callback);

		Mesh* Create(const std::string& name, std::vector<float>& vertexAttributes, 
			std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath = "None");

		void CreateAsync(const std::string& name, std::vector<float>& vertexAttributes,
			std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, std::function<void(Mesh*)> callback, const std::string& filePath = "None");

		Mesh* Get(const std::string& name) const;

		void Delete(Mesh* mesh);

		void ShutDown();
	};

}
