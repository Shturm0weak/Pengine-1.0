#pragma once

#include "Core.h"
#include "../OpenGL/Material.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API MaterialManager
	{
	private:

		std::unordered_map<std::string, Material*> m_MaterialsByFilePath;

		MaterialManager() = default;
		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator=(const MaterialManager&) { return *this; }
		~MaterialManager() = default;

		friend class Editor;
	public:

		static MaterialManager& GetInstance();
		
		Material* Get(const std::string& filePath);

		Material* Load(const std::string& filePath, bool reload = false);

		void Delete(Material* material);

		void ShutDown();
	};

}
