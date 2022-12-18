#pragma once

#include "Core.h"
#include "Asset.h"
#include "TextureManager.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API TextureAtlas : public IAsset
	{
	private:

		glm::vec2 m_Size = { 0.0f, 0.0f };
		Texture* m_Texture = TextureManager::GetInstance().White();

		TextureAtlas(const std::string& name, const glm::vec2& size, Texture* texture);

		friend class TextureAtlasManager;
		friend class Serializer;
	public:

		std::vector<float> GetUV(const glm::vec2& position) const;
		
		void SetTexture(Texture* texture) { m_Texture = texture; }

		Texture* GetTexture() const { return m_Texture; }
		
		void SetSize(const glm::vec2& size) { m_Size = size; }

		glm::vec2 GetSize() const { return m_Size; }
	};

	struct PENGINE_API TextureAtlasData
	{
	public:
		TextureAtlas* m_TextureAtlas = nullptr;
		int m_I = 0, m_J = 0;

		TextureAtlasData(TextureAtlas* textureAtlas, int i, int j) : m_TextureAtlas(textureAtlas), m_I(i), m_J(j)
		{}
	};

}