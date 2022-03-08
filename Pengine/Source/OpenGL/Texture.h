#pragma once

#include "../Core/Core.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Texture
	{
	public:

		struct PENGINE_API TexParameteri
		{
			GLenum m_Target;
			GLenum m_Pname;
			GLint m_Param;
		};
	private:

		std::string m_FilePath;
		std::string m_Name;
		unsigned char* m_LocalBuffer = nullptr;
		uint32_t m_RendererID = UINT_MAX;
		int m_width = 0, m_height = 0, m_BPP = 0;

		bool LoadInRAM(bool flip = true);
		bool LoadInVRAM(const std::vector<TexParameteri>& texParameters, bool unloadFromRAM = true);
		void UnLoadFromRAM();
		void UnLoadFromVRAM();
		void ColoredTexture(const std::vector<TexParameteri>& texParameters, uint32_t color);

		friend class TextureManager;
	public:

		Texture(const std::string& filePath);
		~Texture();

		void Bind(unsigned int slot = 0) const;
		void UnBind() const;

		uint32_t GetRendererID() const { return m_RendererID; }
		std::string GetName() const { return m_Name; }
		std::string GetFilePath() const { return m_FilePath; }
		glm::vec2 GetSize() const { return { m_width, m_height }; }
	};

}