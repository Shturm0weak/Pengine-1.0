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
			uint32_t m_FrameBufferAttachment = GL_COLOR_ATTACHMENT0;
			uint32_t m_TextureInternalFormat = GL_RGBA;
			uint32_t m_TextureFormat = GL_RGBA;
			uint32_t m_TextureType = GL_UNSIGNED_BYTE;
		};

		std::vector<FrameBufferParams> m_Params;
		uint32_t m_Fbo = 0;
		uint32_t m_Rbo = 0;
		std::vector<unsigned int> m_Textures;

		FrameBuffer(const std::vector<FrameBufferParams>& params, const std::vector<Texture::TexParameteri>& texParameters);
		~FrameBuffer();

		void Resize(const glm::ivec2& size);
		
		void GenerateRbo();

		void Bind() const;
		
		void UnBind() const;
	};

}