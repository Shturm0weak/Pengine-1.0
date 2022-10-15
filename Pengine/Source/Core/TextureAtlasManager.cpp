#include "TextureAtlasManager.h"

#include "Serializer.h"
#include "Utils.h"

using namespace Pengine;

TextureAtlasManager& TextureAtlasManager::GetInstance()
{
    static TextureAtlasManager textureAtlasManager;
    return textureAtlasManager;
}

TextureAtlas* TextureAtlasManager::Load(const std::string& filePath)
{
	auto textureAtlasIter = m_TextureAtlases.find(Utils::GetNameFromFilePath(filePath));
	if (textureAtlasIter != m_TextureAtlases.end())
	{
		TextureAtlas* textureAtlas = Serializer::DeSerializeTextureAtlas(filePath);
		delete textureAtlasIter->second;
		textureAtlasIter->second = textureAtlas;
		return textureAtlas;
	}
	else
	{
		TextureAtlas* textureAtlas = Serializer::DeSerializeTextureAtlas(filePath);
		return textureAtlas;
	}
}

TextureAtlas* TextureAtlasManager::Create(const std::string& name, const glm::vec2& size, Texture* texture)
{
	TextureAtlas* textureAtlas = Get(name);
	if (textureAtlas)
	{
		Delete(textureAtlas);
	}
	textureAtlas = new TextureAtlas(name, size, texture);
	m_TextureAtlases.insert(std::make_pair(textureAtlas->m_Name, textureAtlas));
	return textureAtlas;
}

TextureAtlas* TextureAtlasManager::Get(const std::string& name)
{
	auto textureAtlasIter = m_TextureAtlases.find(name);
	if (textureAtlasIter != m_TextureAtlases.end())
	{
		return textureAtlasIter->second;
	}
	else
	{
		return nullptr;
	}
}

void TextureAtlasManager::ShutDown()
{
	std::vector<TextureAtlas*> textureAtlases;
	for (auto i = m_TextureAtlases.begin(); i != m_TextureAtlases.end(); i++)
	{
		textureAtlases.push_back(i->second);
	}
	m_TextureAtlases.clear();

	for (uint32_t i = 0; i < textureAtlases.size(); i)
	{
		delete textureAtlases[i];
		textureAtlases.erase(textureAtlases.begin() + i);
	}
}

void TextureAtlasManager::Delete(TextureAtlas* textureAtlas)
{
	auto iter = m_TextureAtlases.find(textureAtlas->m_Name);
	if (iter != m_TextureAtlases.end())
	{
		delete iter->second;
		m_TextureAtlases.erase(iter);
	}
}
