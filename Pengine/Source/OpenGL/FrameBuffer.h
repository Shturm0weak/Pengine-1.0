#pragma once

#include "../Core/Core.h"
#include "../Core/TextureManager.h"

#include <vector>

namespace Pengine
{

	class PENGINE_API FrameBuffer {
	public:

		struct FrameBufferParams
		{
			glm::ivec2 m_Size;
			uint32_t m_TexturesAmount = 1;
			int m_FrameBufferAttachment = GL_COLOR_ATTACHMENT0;
			int m_TextureInternalFormat = GL_RGBA;
			int m_TextureFormat = GL_RGBA;
			int TextureType = GL_UNSIGNED_BYTE;
			bool m_HasRBO = false;
			bool m_ReadBuffer = true;
			bool m_DrawBuffer = true;
		};

		FrameBufferParams m_Params;
		uint32_t m_Rbo = 0;
		uint32_t m_Fbo = 0;
		std::vector<unsigned int> m_Textures;

		FrameBuffer(const FrameBufferParams& params, const std::vector<Texture::TexParameteri>& texParameters);
		~FrameBuffer();

		void Resize(const glm::ivec2& size);
		
		void Bind() const;
		
		void UnBind() const;
	};

}