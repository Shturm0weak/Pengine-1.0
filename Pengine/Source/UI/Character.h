#pragma once

#include "../Core/Core.h"

#include <unordered_map>

namespace Pengine {

	class Font;
	class Character;

	struct PENGINE_API Character {
	private:

		float m_Mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		glm::vec4 m_Color = glm::vec4(1.0f);
		glm::vec2 m_Position;
		glm::vec2 m_Scale;

		float m_XOffset;
		float m_YOffset;
		float m_XAdvance;
		int m_Id;
		int m_X;
		int m_Y;
		int m_Width;
		int m_Height;
		
		friend class Gui;
		friend class Batch;
		friend class Font;
	public:

		Font* m_Font = nullptr;
		wchar_t m_Char;
	public:

		void Initialize(Font* font, wchar_t ch, const glm::vec2& position = { 0.0f, 0.0f }, float scale = 1.0f);
	};

	class PENGINE_API Font {
	private:

		Character* m_CharacterPtr = nullptr;
		wchar_t* m_Id;
		int* m_X;
		int* m_Y;
		int* m_Width;
		int* m_Height;
		int* m_XOffset;
		int* m_YOffset;
		int* m_XAdvance;
	public:

		std::unordered_map<wchar_t, Character*> m_Characters;
		std::string m_Name = "";
		
		class Texture* m_FontAtlas = nullptr;

		uint32_t m_Count = 0;
		uint32_t m_Size = 0;
	public:

		void LoadFont(const std::string& filename, const std::string& pathToTextureAtlas);
		
		void LoadCharacters();

		~Font();
	
		friend class Character;
	};

}