#pragma once

#include "Core.h"
#include "../OpenGL/Texture.h"

#include <functional>
#include <unordered_map>
#include <map>

namespace Pengine
{

	class PENGINE_API TextureManager
	{
	private:

		std::multimap<std::string, std::function<void(Texture*)>> m_WaitingForTextures;
		std::unordered_map<std::string, Texture*> m_Textures;
		std::vector<Texture::TexParameteri> m_TexParameters;
		
		void DispatchLoadedTextures();

		TextureManager() = default;
		TextureManager(const TextureManager&) = delete;
		TextureManager& operator=(const TextureManager&) { return *this; }
		~TextureManager() = default;

		friend class EntryPoint;
		friend class Editor;
		friend class Renderer;
		friend class Viewport;
	public:

		static TextureManager& GetInstance();

		void ShutDown();
		
		void Delete(Texture* texture);
		
		void AsyncCreate(const std::string& filePath);
		
		Texture* White() const { return GetByName("White"); }

		Texture* Create(const std::string& filePath, bool flip = true);
		
		Texture* ColoredTexture(const std::string& name, uint32_t color);
		
		Texture* GetByFilePath(const std::string& filePath, bool showErrors = false) const;
		
		Texture* GetByName(const std::string& name, bool showErrors = false) const;

		void AsyncGetByFilePath(std::function<void(Texture*)> callback, const std::string& filePath);
		
		void AsyncGetByName(std::function<void(Texture*)> callback, const std::string& name);
		
		void RemoveFromGetAsync(const std::string& key);

		void ResetTexParametersi();
		
		void ReloadAllTextures();

		std::vector<Texture*> GetTexturesFromFolder(const std::string& filePath);
		
		std::vector<Texture::TexParameteri> GetTexParamertersi() const { return m_TexParameters; }
	};

}