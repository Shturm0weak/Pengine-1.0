#pragma once

#include "Core.h"
#include "Mesh.h"

#include <unordered_map>

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
		Mesh* Create(const std::string& name, std::vector<float>& vertexAttributes, 
			std::vector<uint32_t>& indices, const std::vector<uint32_t>& layouts, const std::string& filePath = "None");
		Mesh* Get(const std::string& name) const;

		void ShutDown();
	};

}
