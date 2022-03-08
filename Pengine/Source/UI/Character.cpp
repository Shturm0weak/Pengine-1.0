#include "Character.h"

#include "../Core/TextureManager.h"
#include "../Core/Logger.h"

#include <fstream>

using namespace Pengine;

void Character::Initialize(Font* font, wchar_t ch, const glm::vec2& position, float scale)
{
	m_Font = font;
	m_Position = position;

	{
		wchar_t i = ch;
		m_Char = font->m_Id[i];
		m_Id = font->m_Id[i];
		m_X = font->m_X[i];
		m_Y = font->m_Y[i];
		m_Width = font->m_Width[i];
		m_Height = font->m_Height[i];
		m_XOffset = font->m_XOffset[i];
		m_YOffset = font->m_YOffset[i];
		m_XAdvance = font->m_XAdvance[i];
	}

	{
		double size = font->m_FontAtlas->GetSize().x;

		m_Mesh2D[2]  = (m_X) / size;
		m_Mesh2D[3]  = (abs(m_Y - size) - m_Height) / size;
		m_Mesh2D[6]  = (m_X + m_Width) / size;
		m_Mesh2D[7]  = (abs(m_Y - size) - m_Height) / size;
		m_Mesh2D[10] = (m_X + m_Width) / size;
		m_Mesh2D[11] = abs(m_Y - size) / size;
		m_Mesh2D[14] = m_X / size;
		m_Mesh2D[15] = abs(m_Y - size) / size;

		// Top-left corner!!!
		m_Mesh2D[0] = 0;
		m_Mesh2D[1] = -(m_Height + m_YOffset);
		m_Mesh2D[4] = (m_Width);
		m_Mesh2D[5] = -(m_Height + m_YOffset);
		m_Mesh2D[8] = (m_Width);
		m_Mesh2D[9] = -m_YOffset;
		m_Mesh2D[12] = 0;
		m_Mesh2D[13] = -m_YOffset;
	}

	m_YOffset *= scale;
	m_Scale.x = scale;
	m_Scale.y = scale;
}

Font::~Font()
{
	delete[] m_CharacterPtr;
	m_Characters.clear();
}

void Font::LoadFont(const std::string& filename, const std::string& pathToTextureAtlas)
{
	m_FontAtlas = TextureManager::GetInstance().Create(pathToTextureAtlas);
	std::ifstream in_file;
	in_file.open(filename);
	if (in_file.is_open()) 
	{
		in_file >> m_Name;
		in_file >> m_Count;
		in_file >> m_Size;
		m_Id = new wchar_t[m_Count];
		m_X = new int[m_Count];
		m_Y = new int[m_Count];
		m_Width = new int[m_Count];
		m_Height = new int[m_Count];
		m_XOffset = new int[m_Count];
		m_YOffset = new int[m_Count];
		m_XAdvance = new int[m_Count];
		for (unsigned int i = 0; i < m_Count; i++)
		{
			uint16_t id;
			in_file >> id;
			m_Id[i] = id;
			in_file >> m_X[i];
			in_file >> m_Y[i];
			in_file >> m_Width[i];
			in_file >> m_Height[i];
			in_file >> m_XOffset[i];
			in_file >> m_YOffset[i];
			in_file >> m_XAdvance[i];
		}
	}
	in_file.close();
	Logger::Success("has been loaded!", "Font", m_Name.c_str());
}

void Font::LoadCharacters()
{
	m_CharacterPtr = new Character[m_Count];
	for (unsigned int i = 0; i < m_Count; i++)
	{
		m_CharacterPtr[i].Initialize(this, (wchar_t)i);
		m_Characters.insert(std::make_pair(m_CharacterPtr[i].m_Char, &m_CharacterPtr[i]));
	}

	delete[] m_Id;
	delete[] m_X;
	delete[] m_Y;
	delete[] m_Width;
	delete[] m_Height;
	delete[] m_XOffset;
	delete[] m_YOffset;
	delete[] m_XAdvance;
}