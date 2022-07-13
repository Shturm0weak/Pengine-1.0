#include "Texture.h"

#include "stb_image.h"
#include "../Core/Utils.h"
#include "../Core/Logger.h"
#include "../Core/ThreadPool.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

bool Texture::LoadInRAM(bool flip)
{
	if (m_LocalBuffer == nullptr)
	{
		stbi_set_flip_vertically_on_load(flip);
		m_LocalBuffer = stbi_load(m_FilePath.c_str(), &m_width, &m_height, &m_BPP, 4);
		return true;
	}
	else
	{
#ifdef _DEBUG
		Logger::Warning("has been already in RAM!", "Texture", m_Name.c_str());
#endif
		return false;
	}
}

bool Texture::LoadInVRAM(const std::vector<TexParameteri>& texParameters, bool unloadFromRAM)
{
	if (m_LocalBuffer == nullptr)
	{
#ifdef _DEBUG
		Logger::Warning("can't be loaded in VRAM, m_LocalBuffer is unloaded from RAM!", "Texture",  m_Name.c_str());
#endif
		return false;
	}

	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	m_IsLinear = texParameters[0].m_Param == GL_LINEAR;

	for (size_t i = 0; i < texParameters.size(); i++)
	{
		glTexParameteri(texParameters[i].m_Target, texParameters[i].m_Pname, texParameters[i].m_Param);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	if (unloadFromRAM)
	{
		UnLoadFromRAM();
	}
	return true;
}

void Texture::UnLoadFromRAM()
{
	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}

void Texture::UnLoadFromVRAM()
{
	glDeleteTextures(1, &m_RendererID);
	m_RendererID = UINT32_MAX;
}

void Texture::ColoredTexture(const std::vector<TexParameteri>& texParameters, uint32_t color)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	for (size_t i = 0; i < texParameters.size(); i++)
	{
		glTexParameteri(texParameters[i].m_Target, texParameters[i].m_Pname, texParameters[i].m_Param);
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string& filePath)
{
	m_FilePath = filePath;
	m_Name = Utils::GetNameFromFilePath(filePath);
}

Texture::~Texture()
{
	UnLoadFromRAM();
	UnLoadFromVRAM();
}

void Texture::Reload()
{
	std::vector<Texture::TexParameteri> params = TextureManager::GetInstance().GetTexParamertersi();
	ThreadPool::GetInstance().Enqueue([=] {
		UnLoadFromRAM();
		LoadInRAM();

		std::function<void()> callback = std::function<void()>([=] {
			UnLoadFromVRAM();
			LoadInVRAM(params);
		});
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
	});
}

void Texture::Bind(unsigned int slot) const
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
