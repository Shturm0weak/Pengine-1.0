#pragma once

#include "Core.h"
#include "ReflectionSystem.h"

namespace Pengine
{

	class PENGINE_API IAsset
	{
		RTTR_ENABLE()

		protected: PROPERTY(IAsset, std::string, m_FilePath ,"None");
		protected: PROPERTY(IAsset, std::string, m_Name, "None");
	public:

		void Copy(const IAsset& asset)
		{
			COPY_PROPERTIES(IAsset, asset)
		}

		void GenerateFromFilePath(const std::string& filePath, size_t formatSize = 3);

		void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }
		
		void SetName(const std::string& name) { m_Name = name; }

		std::string GetFilePath() const { return m_FilePath; }

		std::string GetName() const { return m_Name; }

		virtual void Reload() = 0;

		virtual ~IAsset() = default;

		friend class TextureManager;
	};
	REGISTER_ASSET(IAsset)

	//class PENGINE_API IAsset
	//{
	//	RTTR_ENABLE()

	//protected:
	//std::string m_FilePath = "None";
	//std::string m_Name = "None";
	//public:

	//	void Copy(const IAsset& asset)
	//	{
	//		m_FilePath = asset.m_FilePath;
	//		m_Name = asset.m_Name;
	//	}


	//	void GenerateFromFilePath(const std::string& filePath, size_t formatSize = 3);

	//	void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }

	//	void SetName(const std::string& name) { m_Name = name; }

	//	std::string GetFilePath() const { return m_FilePath; }

	//	std::string GetName() const { return m_Name; }

	//	virtual void Reload() = 0;

	//	virtual ~IAsset() = default;

	//	friend class TextureManager;
	//};

}
