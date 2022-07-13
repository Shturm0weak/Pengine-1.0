#pragma once

#include "Core.h"
#include "TextureAtlas.h"

#include <unordered_map>

namespace Pengine
{

	class PENGINE_API TextureAtlasManager
	{
	private:

		std::unordered_map<std::string, class TextureAtlas*> m_TextureAtlases;

		TextureAtlasManager() = default;
		TextureAtlasManager(const TextureAtlasManager&) = delete;
		TextureAtlasManager& operator=(const TextureAtlasManager&) { return *this; }
		~TextureAtlasManager() = default;
	public:

		static TextureAtlasManager& GetInstance();

		TextureAtlas* Load(const std::string& filePath);
		
		TextureAtlas* Create(const std::string& name, const glm::vec2& size = { 0.0f, 0.0f }, class Texture* texture = TextureManager::GetInstance().Get("White"));
		
		TextureAtlas* Get(const std::string& name);
		
		void ShutDown();
		
		void Delete(TextureAtlas* textureAtlas);
	};

}
