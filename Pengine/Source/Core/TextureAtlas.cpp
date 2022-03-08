#include "TextureAtlas.h"

#include "TextureManager.h"

using namespace Pengine;

TextureAtlas::TextureAtlas(const std::string& name, const glm::vec2& size, class Texture* texture)
	: m_Name(name)
	, m_Size(size)
	, m_Texture(texture)
{
	m_FilePath = "Source/TextureAtlases/" + m_Name + ".ta";
}

std::vector<float> TextureAtlas::GetUV(const glm::vec2& position) const
{
	std::vector<float> uv(8);
	uv[0] = (position.x * m_Size.x) / m_Texture->GetSize().x;
	uv[1] = (position.y * m_Size.y) / m_Texture->GetSize().y;
	uv[2] = ((position.x + 1) * m_Size.x) / m_Texture->GetSize().x;
	uv[3] = (position.y * m_Size.y) / m_Texture->GetSize().y;
	uv[4] = ((position.x + 1) * m_Size.x) / m_Texture->GetSize().x;
	uv[5] = ((position.y + 1) * m_Size.y) / m_Texture->GetSize().y;
	uv[6] = (position.x * m_Size.x) / m_Texture->GetSize().x;
	uv[7] = ((position.y + 1) * m_Size.y) / m_Texture->GetSize().y;
	return uv;
}