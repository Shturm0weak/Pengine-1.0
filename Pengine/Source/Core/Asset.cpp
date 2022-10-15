#include "Asset.h"

#include "Utils.h"

using namespace Pengine;

void IAsset::GenerateFromFilePath(const std::string& filePath)
{
	if (filePath.empty())
	{
		return;
	}

	m_FilePath = filePath;
	m_Name = Utils::GetNameFromFilePath(filePath);
}