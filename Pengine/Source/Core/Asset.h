#pragma once

#include "Core.h"

#include <rttr/registration>

namespace Pengine
{

	class PENGINE_API IAsset
	{
		RTTR_ENABLE()

	protected:

		std::string m_FilePath = "None";
		std::string m_Name = "None";
	public:

		void SetFilePath(const std::string& filePath) { m_FilePath = filePath; }
		
		void SetName(const std::string& name) { m_Name = name; }

		std::string GetFilePath() const { return m_FilePath; }

		std::string GetName() const { return m_Name; }

		virtual void Reload() = 0;

		virtual ~IAsset() = default;

		friend class TextureManager;
	};

}
