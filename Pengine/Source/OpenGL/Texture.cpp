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
		m_LocalBuffer = stbi_load(m_Meta.m_FilePath.c_str(), &m_width, &m_height, &m_BPP, 4);
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

bool Texture::LoadInVRAM(const std::vector<TexParameteri>& texParameters, const std::vector<int>& texParametersIndices, bool unloadFromRAM)
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

	m_Meta.m_Name = m_Name;
	m_Meta.m_FilePath = m_FilePath;

	for (size_t i = 0; i < texParametersIndices.size(); i++)
	{
		size_t index = texParametersIndices[i];
		m_Meta.m_Params[index] = texParameters[index].m_Param;
		glTexParameteri(texParameters[index].m_Target, texParameters[index].m_Pname, texParameters[index].m_Param);
	}

	GLfloat maxAnisotropy;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void Texture::ColoredTexture(const std::vector<TexParameteri>& texParameters, const std::vector<int>& texParametersIndices, uint32_t color)
{
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	for (size_t i = 0; i < texParametersIndices.size(); i++)
	{
		size_t index = texParametersIndices[i];
		m_Meta.m_Params[index] = texParameters[index].m_Param;
		glTexParameteri(texParameters[index].m_Target, texParameters[index].m_Pname, texParameters[index].m_Param);
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

void Texture::Reload(const std::vector<Texture::TexParameteri>& params, const std::vector<int>& texParametersIndices)
{
	if (m_FilePath == "None") return;

	ThreadPool::GetInstance().Enqueue([=] {
		UnLoadFromRAM();
		LoadInRAM();

		std::function<void()> callback = std::function<void()>([=] {
			UnLoadFromVRAM();
			LoadInVRAM(params, texParametersIndices);
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

Texture::Meta Texture::GenerateMeta()
{
	Meta meta;

	meta.m_Name = m_Name;
	meta.m_FilePath = m_FilePath;
	meta.m_Params = m_Meta.m_Params;

	return meta;
}

void Texture::Reload()
{
	if (m_FilePath == "None") return;

	ThreadPool::GetInstance().Enqueue([=] {
		UnLoadFromRAM();
		LoadInRAM();

		std::function<void()> callback = std::function<void()>([=] {
			UnLoadFromVRAM();

			LoadInVRAM(
			{ 
				{ GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_Meta.m_Params[0] },
				{ GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_Meta.m_Params[1]},
				{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_Meta.m_Params[2] },
				{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_Meta.m_Params[3] }
			},
			TextureManager::GetInstance().GetDefaultTexParamertersIndices());
		});
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
	});
}
