#pragma once

#include "../Core/Core.h"
#include "../Core/Asset.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Texture : public IAsset
	{
	public:

		struct PENGINE_API TexParameteri
		{
			GLenum m_Target;
			GLenum m_Pname;
			GLint m_Param;
		};
	private:

		unsigned char* m_LocalBuffer = nullptr;
		uint32_t m_RendererID = UINT_MAX;
		int m_width = 0, m_height = 0, m_BPP = 0;

		struct PENGINE_API Meta
		{
			std::string m_FilePath;
			std::string m_Name;
			std::vector<int> m_Params = std::vector<int>(4);
		} m_Meta;

		bool LoadInRAM(bool flip = true);
		bool LoadInVRAM(const std::vector<TexParameteri>& texParameters, const std::vector<int>& texParametersIndices, bool unloadFromRAM = true);

		void UnLoadFromRAM();
		void UnLoadFromVRAM();

		void ColoredTexture(const std::vector<TexParameteri>& texParameters, const std::vector<int>& texParametersIndices, uint32_t color);

		friend class TextureManager;
		friend class Editor;
		friend class Serializer;
	public:

		Texture(const std::string& filePath);
		~Texture();

		void Reload();

		void Reload(const std::vector<Texture::TexParameteri>& params, const std::vector<int>& texParametersIndices);
		
		void Bind(unsigned int slot = 0) const;
		
		void UnBind() const;

		uint32_t GetRendererID() const { return m_RendererID; }
		
		glm::vec2 GetSize() const { return { m_width, m_height }; }
		
		Meta GenerateMeta();

	};

}