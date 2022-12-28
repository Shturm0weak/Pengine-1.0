#pragma once

#include "Core.h"
#include "ReflectionSystem.h"

namespace Pengine
{

	class PENGINE_API IAsset
	{
	protected: PROPERTY(std::string, m_Name, none)
	protected: PROPERTY(std::string, m_FilePath, none)
	public:

		std::string GetName() const { return m_Name; }
		std::string GetFilePath() const { return m_FilePath; }

		void GenerateFromFilePath(const std::string& filePath);
	};
	REGISTER_CLASS(IAsset)

}
