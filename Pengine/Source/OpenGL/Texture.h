#pragma once

#include "../Core/Core.h"
#include "../Core/Asset.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API Texture : public IAsset
	{
		RTTR_ENABLE(IAsset)

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
		bool m_IsLinear = false;

		bool LoadInRAM(bool flip = true);
		bool LoadInVRAM(const std::vector<TexParameteri>& texParameters, bool unloadFromRAM = true);

		void UnLoadFromRAM();
		void UnLoadFromVRAM();

		void ColoredTexture(const std::vector<TexParameteri>& texParameters, uint32_t color);

		friend class TextureManager;
		friend class Editor;
	public:

		Texture(const std::string& filePath);
		~Texture();

		virtual void Reload() override;
		
		void Bind(unsigned int slot = 0) const;
		
		void UnBind() const;

		uint32_t GetRendererID() const { return m_RendererID; }
		
		glm::vec2 GetSize() const { return { m_width, m_height }; }
		
		bool IsLinear() const { return m_IsLinear; }
	};

}