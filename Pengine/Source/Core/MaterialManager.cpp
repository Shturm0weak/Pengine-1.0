#include "MaterialManager.h"

#include "Serializer.h"
#include "../OpenGL/Material.h"
#include "../OpenGL/BaseMaterial.h"

using namespace Pengine;

Material* MaterialManager::Get(const std::string& filePath)
{
    auto& materialByFilePath = m_MaterialsByFilePath.find(filePath);
    if (materialByFilePath != m_MaterialsByFilePath.end())
    {
        return materialByFilePath->second;
    }
    else
    {
        return nullptr;
    }
}

BaseMaterial* MaterialManager::GetBase(const std::string& filePath)
{
    auto& materialByFilePath = m_BaseMaterialsByFilePath.find(filePath);
    if (materialByFilePath != m_BaseMaterialsByFilePath.end())
    {
        return materialByFilePath->second;
    }
    else
    {
        return nullptr;
    }
}

Material* MaterialManager::Load(const std::string& filePath)
{
    std::string formattedFilePath = Utils::Replace(filePath, '\\', '/');
    Material* material = Get(formattedFilePath);
    if (!material)
    {
        material = Serializer::DeserializeMaterial(formattedFilePath);
        m_MaterialsByFilePath.emplace(formattedFilePath, material);
        m_BaseMaterialsByFilePath.emplace(material->m_BaseMaterial->GetFilePath(), material->m_BaseMaterial);
    }

    return material;
}

BaseMaterial* MaterialManager::LoadBase(const std::string& filePath)
{
    std::string formattedFilePath = Utils::Replace(filePath, '\\', '/');
    BaseMaterial* material = GetBase(formattedFilePath);
    if (!material)
    {
        material = Serializer::DeserializeBaseMaterial(formattedFilePath);
        m_BaseMaterialsByFilePath.emplace(material->GetFilePath(), material);
    }

    return material;
}

void MaterialManager::Delete(Material* material)
{
    if (material)
    {
        m_MaterialsByFilePath.erase(material->GetFilePath());
        delete material;
    }
}

void MaterialManager::ShutDown()
{
    for (auto& materialByFilePath : m_MaterialsByFilePath)
    {
        delete materialByFilePath.second;
    }
}

MaterialManager& MaterialManager::GetInstance()
{
    static MaterialManager materialManager;
    return materialManager;
}
