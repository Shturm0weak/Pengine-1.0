#include "TextureManager.h"

#include "ThreadPool.h"
#include "Logger.h"
#include "Utils.h"
#include "Serializer.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

void TextureManager::DispatchLoadedTextures()
{
	for (auto i = m_WaitingForTextures.begin(); i != m_WaitingForTextures.end();)
	{
		std::function<void(Texture*)> callback = i->second;
		Texture* texture = GetByFilePath(i->first);

		if (!texture) texture = GetByName(i->first);

		if (texture != nullptr)
		{
			if (callback)
			{
				callback(texture);
			}
			m_WaitingForTextures.erase(i++);
		}
		else
		{
			++i;
		}
	}
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager textureManager;
	return textureManager;
}

void TextureManager::ShutDown()
{
	std::vector<Texture*> textures;
	for (auto i = m_Textures.begin(); i != m_Textures.end(); i++)
	{
		textures.push_back(i->second);
	}
	m_Textures.clear();

	for (uint32_t i = 0; i < textures.size(); i)
	{
		delete textures[i];
		textures.erase(textures.begin() + i);
	}
}

void TextureManager::Delete(Texture* texture)
{
	auto iter = m_Textures.find(texture->GetName());
	if (iter != m_Textures.end())
	{
		delete iter->second;
		m_Textures.erase(iter);
	}
}

void TextureManager::AsyncLoad(const std::string& filePath, std::function<void(Texture*)> callback)
{
	if (!Utils::IsTextureFile(filePath) && !Utils::Contains(filePath, ".meta"))
	{
		return;
	}

	std::string fomattedFilePath = Utils::Replace(filePath, '\\', '/');
	std::string metaFilePath = Utils::ReplaceFormat(fomattedFilePath, ".meta");

	Texture::Meta meta = Serializer::DeserializeTextureMeta(metaFilePath);

	if (meta.m_Name == "")
	{
		meta = GenerateTextureMeta(filePath);
		Serializer::SerializeTextureMeta(meta);
	}

	Texture* texture = nullptr;

	if (texture = GetByFilePath(meta.m_FilePath))
	{
		callback(texture);

		return;
	}

	if (Utils::IsThere<std::string>(m_TexturesIsLoading, meta.m_FilePath))
	{
		m_WaitingForTextures.emplace(meta.m_FilePath, callback);

		return;
	}

	m_TexturesIsLoading.emplace_back(meta.m_FilePath);

	ThreadPool::GetInstance().Enqueue([=] {
		Texture* texture = new Texture(meta.m_FilePath);
		texture->m_Meta = meta;

		texture->LoadInRAM();

		std::vector<Texture::TexParameteri> params =
		{
			{ GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, meta.m_Params[0] },
			{ GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, meta.m_Params[1]},
			{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, meta.m_Params[2] },
			{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, meta.m_Params[3] }
		};

		std::function<void()> mainThreadCallback = std::function<void()>([=] {
			texture->LoadInVRAM(params, GetDefaultTexParamertersIndices());
			m_Textures.insert(std::make_pair(meta.m_FilePath, texture));
			DispatchLoadedTextures();
			callback(texture);
			Utils::Erase<std::string>(m_TexturesIsLoading, meta.m_FilePath);
		});
		EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(mainThreadCallback, EventType::ONMAINTHREADPROCESS));
	});
}

void TextureManager::Create(const std::string& filePath, std::function<void(Texture*)> callback, bool flip)
{
	if (!Utils::IsTextureFile(filePath) && !Utils::Contains(filePath, ".meta"))
	{
		return;
	}

	std::string fomattedFilePath = Utils::Replace(filePath, '\\', '/');
	std::string metaFilePath = Utils::ReplaceFormat(fomattedFilePath, ".meta");

	Texture::Meta meta = Serializer::DeserializeTextureMeta(metaFilePath);

	if (meta.m_Name == "")
	{
		meta = GenerateTextureMeta(filePath);
		Serializer::SerializeTextureMeta(meta);
	}

	Texture* texture = nullptr;

	if (texture = GetByFilePath(meta.m_FilePath))
	{
		callback(texture);

		return;
	}

	if (Utils::IsThere<std::string>(m_TexturesIsLoading, meta.m_FilePath))
	{
		if (callback)
		{
			m_WaitingForTextures.emplace(meta.m_FilePath, callback);
		}

		return;
	}

	m_TexturesIsLoading.emplace_back(meta.m_FilePath);

	texture = new Texture(meta.m_FilePath);
	texture->m_Meta = meta;

	texture->LoadInRAM();

	std::vector<Texture::TexParameteri> params =
	{
		{ GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, meta.m_Params[0] },
		{ GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, meta.m_Params[1]},
		{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, meta.m_Params[2] },
		{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, meta.m_Params[3] }
	};

	texture->LoadInVRAM(params, GetDefaultTexParamertersIndices());
	m_Textures.insert(std::make_pair(meta.m_FilePath, texture));
	Utils::Erase<std::string>(m_TexturesIsLoading, meta.m_FilePath);
	DispatchLoadedTextures();

	callback(texture);
}

Texture* TextureManager::ColoredTexture(const std::string& name, uint32_t color)
{
	Texture* texture = GetByName(name);
	if (texture)
	{
#ifdef _DEBUG
		Logger::Warning("has already existed!", "Texture", name.c_str());
#endif
		return texture;
	}

	texture = new Texture(name);
	texture->m_Name = name;
	texture->ColoredTexture(m_TexParameters, GetDefaultTexParamertersIndices(), color);

	m_Textures.insert(std::make_pair(texture->GetFilePath(), texture));

	return texture;
}

Texture* TextureManager::GetByFilePath(const std::string& filePath, bool showErrors) const
{
	if (filePath == "White")
	{
		return White();
	}

	auto textureIter = m_Textures.find(filePath);
	if (textureIter != m_Textures.end())
	{
		return textureIter->second;
	}
	else
	{
		if (showErrors)
		{
			Logger::Warning("doesn't exist!", "Texture", filePath.c_str());
		}

		return nullptr;
	}
}

Texture* TextureManager::GetByName(const std::string& name, bool showErrors) const
{
	for (auto textureIter : m_Textures)
	{
		if (textureIter.second->GetName() == name)
		{
			return textureIter.second;
		}
	}

	if (showErrors)
	{
		Logger::Warning("doesn't exist!", "Texture", name.c_str());
	}

	return nullptr;
}

void TextureManager::AsyncGetByFilePath(std::function<void(Texture*)> callback, const std::string& filePath)
{
	Texture* texture = GetByFilePath(filePath);
	if (texture != nullptr)
	{
		callback(texture);
	}
	else
	{
		m_WaitingForTextures.insert(std::make_pair(filePath, callback));
	}
}

void TextureManager::AsyncGetByName(std::function<void(Texture*)> callback, const std::string& name)
{
	Texture* texture = GetByName(name);
	if (texture != nullptr)
	{
		callback(texture);
	}
	else
	{
		m_WaitingForTextures.insert(std::make_pair(name, callback));
	}
}

void TextureManager::RemoveFromGetAsync(const std::string& key)
{
	for (auto i = m_WaitingForTextures.begin(); i != m_WaitingForTextures.end();)
	{
		if (i->first == key)
		{
			m_WaitingForTextures.erase(i++);
		}
		else
		{
			++i;
		}
	}
}

void TextureManager::ResetTexParametersi()
{
	m_TexParameters.resize(4);
	m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR };
	m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR };
	m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
	m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
}

void TextureManager::ReloadAllTextures()
{
	for (auto texture : m_Textures)
	{
		if (texture.second->GetFilePath() != "White")
		{
			texture.second->Reload();
		}
	}
}

Texture::Meta TextureManager::GenerateTextureMeta(const std::string& filePath)
{
	Texture::Meta meta;
	meta.m_FilePath = filePath;
	meta.m_Name = Utils::GetNameFromFilePath(filePath);
	meta.m_Params = GetMetaTexParams();

	return meta;
}

std::vector<Texture*> TextureManager::GetTexturesFromFolder(const std::string& filePath)
{
	std::vector<Texture*> textures;
	std::filesystem::path path(filePath);

	for (auto& directoryIter : std::filesystem::directory_iterator(path))
	{
		std::string filename = directoryIter.path().string();
		if (Utils::Contains(filename, "png") || Utils::Contains(filename, "jpg"))
		{
			Create(filename,
				[&](Texture* texture)
			{
				textures.push_back(texture);
			});
		}
	}

	return textures;
}

std::vector<int> TextureManager::GetMetaTexParams() const
{
	std::vector<int> params(m_TexParameters.size());
	for (size_t i = 0; i < m_TexParameters.size(); i++)
	{
		params[i] = m_TexParameters[i].m_Param;
	}

	return params;
}

std::vector<int> TextureManager::GetDefaultTexParamertersIndices() const
{
	std::vector<int> indices(4);
	for (size_t i = 0; i < 4; i++)
	{
		indices[i] = i;
	}

	return indices;
}
