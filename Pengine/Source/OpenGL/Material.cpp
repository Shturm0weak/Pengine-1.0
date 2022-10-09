#include "Material.h"

using namespace Pengine;

Material* Material::Inherit()
{
    Material* material = new Material(*this);
    material->m_IsInherited = true;

    return material;
}

void Material::SetBaseColor(Texture* texture, const std::string& filePath)
{
    m_BaseColorFilePath = filePath;
    m_BaseColor = texture;
}
