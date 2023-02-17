#pragma once

#include "Core.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API MaterialManager
	{
	private:

		std::unordered_map<std::string, class Material*> m_MaterialsByFilePath;
		std::unordered_map<std::string, class BaseMaterial*> m_BaseMaterialsByFilePath;

		MaterialManager() = default;
		MaterialManager(const MaterialManager&) = delete;
		MaterialManager& operator=(const MaterialManager&) { return *this; }
		~MaterialManager() = default;

		friend class Editor;
	public:

		static MaterialManager& GetInstance();
		
		class Material* Get(const std::string& filePath);

		class BaseMaterial* GetBase(const std::string& filePath);

		class Material* Load(const std::string& filePath);

		class BaseMaterial* LoadBase(const std::string& filePath);

		std::unordered_map<std::string, class Material*> GetMaterials() const { return m_MaterialsByFilePath; }

		void Delete(Material* material);

		void ShutDown();
	};

}
