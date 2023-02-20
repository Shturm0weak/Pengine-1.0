#include "BaseMaterial.h"

#include "../Core/Logger.h"
#include "../Core/Renderer.h"

using namespace Pengine;

int BaseMaterial::BindTexture(uint32_t id, GLenum target)
{
    int textureIndex = 0;

    auto instancedObject = std::find(m_TextureSlots.begin(), m_TextureSlots.end(), id);
    if (instancedObject != m_TextureSlots.end())
    {
        textureIndex = (instancedObject - m_TextureSlots.begin());
    }
    else
    {
        if (m_TextureSlotsIndex >= MAX_TEXTURE_SLOTS)
        {
            Logger::Warning("limit of texture slots, texture will be set to white!");
        }
        else
        {
            textureIndex = m_TextureSlotsIndex;
            m_TextureSlots[m_TextureSlotsIndex] = id;
            m_TextureSlotsTarget[m_TextureSlotsIndex] = target;
            m_TextureSlotsIndex++;
        }
    }

    return textureIndex;
}

std::vector<int> BaseMaterial::BindTextures()
{
    std::vector<int> samplers;
    samplers.resize(MAX_TEXTURE_SLOTS);

    Renderer::GetInstance().ClearTextureBindings();

    for (size_t i = 0; i < m_TextureSlotsIndex; i++)
    {
        Renderer::GetInstance().BindTexture(m_TextureSlots[i], -1, m_TextureSlotsTarget[i]);
    }
    for (size_t i = 0; i < MAX_TEXTURE_SLOTS; i++)
    {
        samplers[i] = i;
    }

    return samplers;
}

void BaseMaterial::UnBindTextures()
{
    m_TextureSlotsIndex = 0;
    for (size_t i = 0; i < MAX_TEXTURE_SLOTS; i++)
    {
        m_TextureSlots[i] = 0;
        m_TextureSlotsTarget[i] = GL_TEXTURE_2D;
    }
}
