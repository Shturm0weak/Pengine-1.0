#include "Asset.h"

#include "Utils.h"

void Pengine::IAsset::GenerateFromFilePath(const std::string& filePath, size_t formatSize)
{
	if (filePath.empty())
	{
		return;
	}

	m_FilePath = filePath;
	m_Name = Utils::GetNameFromFilePath(filePath, formatSize);
}