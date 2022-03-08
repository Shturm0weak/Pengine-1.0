#pragma once

#include "Core.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API TextureAtlas
	{
	private:

		std::string m_Name;
		std::string m_FilePath;
		glm::vec2 m_Size;
		class Texture* m_Texture = nullptr;

		TextureAtlas(const std::string& name, const glm::vec2& size, class Texture* texture);

		friend class TextureAtlasManager;
		friend class Serializer;
	public:

		std::vector<float> GetUV(const glm::vec2& position) const;
		std::string GetFilePath() const { return m_FilePath; }
		Texture* GetTexture() const { return m_Texture; }
		std::string GetName() const { return m_Name; }
		glm::vec2 GetSize() const { return m_Size; }
	};

}