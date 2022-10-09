#include "TextureManager.h"

#include "ThreadPool.h"
#include "Logger.h"
#include "Utils.h"
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
			callback(texture);
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

void TextureManager::AsyncCreate(const std::string& filePath)
{
	ThreadPool::GetInstance().Enqueue([=] {
		Texture* texture = GetByFilePath(filePath);
		if (texture == nullptr)
		{
			if (!Utils::MatchType(filePath, { "jpeg", "png", "jpg" })) return;
			
			texture = new Texture(filePath);

			texture->LoadInRAM();

			std::vector<Texture::TexParameteri> params = m_TexParameters;

			std::function<void()> callback = std::function<void()>([=] {
				texture->LoadInVRAM(params, GetDefaultTexParamertersIndices());
				m_Textures.insert(std::make_pair(texture->GetFilePath(), texture));
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
		else
		{
			std::function<void()> callback = std::function<void()>([=] {
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
	});
}

void TextureManager::AsyncCreate(Texture::Meta meta)
{
	ThreadPool::GetInstance().Enqueue([=] {
		Texture* texture = GetByFilePath(meta.m_FilePath);
		if (texture == nullptr)
		{
			if (!Utils::MatchType(meta.m_FilePath, { "jpeg", "png", "jpg" })) return;

			texture = new Texture(meta.m_FilePath);

			texture->LoadInRAM();

			std::vector<Texture::TexParameteri> params =
			{
				{ GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, meta.m_Params[0] },
				{ GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, meta.m_Params[1]},
				{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, meta.m_Params[2] },
				{ GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, meta.m_Params[3] }
			};

			std::function<void()> callback = std::function<void()>([=] {
				texture->LoadInVRAM(params, GetDefaultTexParamertersIndices());
				m_Textures.insert(std::make_pair(texture->GetFilePath(), texture));
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
		else
		{
			std::function<void()> callback = std::function<void()>([=] {
				texture->Reload();
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
	});
}

Texture* TextureManager::Create(const std::string& filePath, bool flip)
{
	Texture* texture = GetByFilePath(filePath);
	if (texture == nullptr)
	{
		if (!Utils::MatchType(filePath, { "jpeg", "png", "jpg" })) return nullptr;
		
		texture = new Texture(filePath);
		m_Textures.insert(std::make_pair(texture->GetFilePath(), texture));

		texture->LoadInRAM();
		texture->LoadInVRAM(m_TexParameters, GetDefaultTexParamertersIndices());
		DispatchLoadedTextures();
		return texture;
	}
	else
	{
		return texture;
	}
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

	if (!Utils::Contains(filePath, ".png") 
		&& !Utils::Contains(filePath, ".jpg") 
		&& !Utils::Contains(filePath, ".jpeg"))
	{
		if (showErrors)
		{
			Logger::Warning("filepath is incorrect!", "Texture", filePath.c_str());
		}

		return nullptr;
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

std::vector<Texture*> TextureManager::GetTexturesFromFolder(const std::string& filePath)
{
	std::vector<Texture*> textures;
	std::filesystem::path path(filePath);

	for (auto& directoryIter : std::filesystem::directory_iterator(path))
	{
		std::string filename = directoryIter.path().string();
		if (Utils::Contains(filename, "png") || Utils::Contains(filename, "jpg"))
		{
			textures.push_back(Create(filename));
		}
	}

	return textures;
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
