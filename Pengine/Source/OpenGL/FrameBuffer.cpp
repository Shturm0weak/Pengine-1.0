#include "FrameBuffer.h"

#include "../Core/Logger.h"

using namespace Pengine;

FrameBuffer::FrameBuffer(const std::vector<FrameBufferParams>& params, const std::vector<Texture::TexParameteri>& texParameters)
{
	m_Params = params;

	glGenFramebuffers(1, &m_Fbo);

	for (uint32_t i = 0; i < params.size(); i++)
	{
		m_Textures.push_back(0);
		glGenTextures(1, &m_Textures[i]);
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			params[i].m_TextureInternalFormat,
			params[i].m_Size.x,
			params[i].m_Size.y,
			0,
			params[i].m_TextureFormat,
			params[i].m_TextureType,
			NULL);

		for (size_t i = 0; i < texParameters.size(); i++)
		{
			glTexParameteri(texParameters[i].m_Target, texParameters[i].m_Pname, texParameters[i].m_Param);
			if (texParameters[i].m_Param == GL_CLAMP_TO_BORDER)
			{
				float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
			}
		}

		glBindTexture(GL_TEXTURE_2D, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
		glFramebufferTexture2D(GL_FRAMEBUFFER, params[i].m_FrameBufferAttachment, GL_TEXTURE_2D, m_Textures[i], 0);
	}

	std::vector<uint32_t> attachments;
	for (FrameBufferParams param : m_Params)
	{
		attachments.emplace_back(param.m_FrameBufferAttachment);
	}
	glDrawBuffers(m_Params.size(), &attachments[0]);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	for (size_t i = 0; i < m_Textures.size(); i++)
	{
		glDeleteTextures(1, &m_Textures[i]);
	}

	glDeleteFramebuffers(1, &m_Fbo);
	glDeleteRenderbuffers(1, &m_Rbo);
}

void FrameBuffer::Resize(const glm::ivec2& size)
{
	for (uint32_t i = 0; i < m_Params.size(); i++)
	{
		m_Params[i].m_Size = size;
		glBindTexture(GL_TEXTURE_2D, m_Textures[i]);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			m_Params[i].m_TextureInternalFormat,
			m_Params[i].m_Size.x,
			m_Params[i].m_Size.y,
			0,
			m_Params[i].m_TextureFormat,
			m_Params[i].m_TextureType,
			NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_Rbo > 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Params[0].m_Size.x, m_Params[0].m_Size.y);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);
		}
	}
}

void FrameBuffer::GenerateRbo()
{
	Bind();
	glGenRenderbuffers(1, &m_Rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_Rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Params[0].m_Size.x, m_Params[0].m_Size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Logger::Error("Framebuffer is not complete!");
	}
}

void FrameBuffer::Bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_Fbo);
}

void FrameBuffer::UnBind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}