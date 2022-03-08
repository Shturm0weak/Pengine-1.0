#include "Gui.h"

#include "../Core/TextureManager.h"
#include "../Core/Input.h"
#include "../Core/Utils.h"
#include "../Core/Window.h"
#include "../Core/Viewport.h"

#include "../OpenGL/Batch.h"

#include "iomanip"
#include "sstream"

#include <functional>

using namespace Pengine;

Gui::Gui()
{
	RecalculateProjectionMatrix();
	LoadStandartFonts();

	TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
		m_CheckBoxTextureTrue = texture;
		}, "Source/UIimages/CheckMarkTrue.png");

	TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
		m_CheckBoxTextureFalse = texture;
		}, "Source/UIimages/CheckMarkFalse.png");

	TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
		m_TriangleRightTexture = texture;
		}, "Source/UIimages/TriangleRight.png");

	TextureManager::GetInstance().AsyncGet([=](Texture* texture) {
		m_TriangleDownTexture = texture;
		}, "Source/UIimages/TriangleDown.png");
}

void Gui::Text(const std::wstring& str, glm::vec2 position, ...)
{
	ApplyRelatedToPanelProperties(&position.x, &position.y);
	glm::vec2 startPos = position;
	float scale = m_Theme.m_FontScale / m_Font->m_Size;
	std::vector<Character*> characters;
	std::vector<size_t> newLines;
	size_t counterAfterDot = 0;
	bool isDotPassed = false;
	va_list argptr;
	va_start(argptr, position);

	size_t strSize = str.size();
	for (size_t i = 0; i < strSize; i++)
	{
		wchar_t strChar = str[i];
		//Not sure about this, maybe it is better to refactor
		if (strChar == L'#')
		{
			if (str[i + 1] == L'%')
			{
				characters.push_back(FindCharInFont(str[i + 1]));
			}
			else if (str[i + 1] == L'#')
			{
				while (str[i + 1] != L' ' && i + 1 != strSize - 1)
				{
					i++;
				}
			}
			i++;
		}
		else if (strChar == L'%')
		{
			std::wstring argument;
			if (str[i + 1] == L'd')
			{
				argument = std::to_wstring(va_arg(argptr, int64_t));
			}
			else if (str[i + 1] == 'f')
			{
				argument = std::to_wstring(va_arg(argptr, double));
			}
			else if (str[i + 1] == 's')
			{
				argument = (va_arg(argptr, std::wstring));
			}

			size_t argumentSize = argument.size();
			for (size_t j = 0; j < argumentSize; j++)
			{
				wchar_t argChar = argument[j];
				if (argChar == L'.' || argChar == L',')
				{
					isDotPassed = true;
					if (m_Theme.m_CharsAfterDot == 0) break;
				}
				if (isDotPassed) counterAfterDot++;
				characters.push_back(FindCharInFont(argChar));
				if (counterAfterDot > m_Theme.m_CharsAfterDot) break;
			}
			isDotPassed = false;
			counterAfterDot = 0;
			i++;
		}
		else if (strChar == L'\n')
		{
			newLines.push_back(characters.size());
			continue;
		}
		else
		{
			characters.push_back(FindCharInFont(strChar));
		}
	}

	//The lowest offset needs to be applied for making the highest character to be aligned with a "zero" line
	float lowestYOffset = 100000.0f; //arbitrary big number
	std::vector<glm::vec2> rowSizes;
	glm::vec2 currentRowSize = { 0.0f, 0.0f };
	glm::vec2 rowsSize = { 0.0f, 0.0f };
	
	size_t charactersSize = characters.size();
	for (size_t i = 0; i < charactersSize; i++)
	{
		Character* character = characters[i];
		character->m_Scale.x = scale;
		character->m_Scale.y = scale;

		float YOffset = character->m_YOffset * character->m_Scale.y;
		float height = character->m_Height * character->m_Scale.y + YOffset;
		if (currentRowSize.y < height) currentRowSize.y = height;
		if (lowestYOffset > YOffset) lowestYOffset = YOffset;
		currentRowSize.x += character->m_XAdvance * character->m_Scale.x;

		auto newLinesIter = std::find(newLines.begin(), newLines.end(), i);
		if (newLinesIter != newLines.end())
		{
			rowSizes.push_back(currentRowSize);
			currentRowSize = { 0.0f, 0.0f };
		}
	}

	if (charactersSize == 0)
	{
		lowestYOffset = 0.0f;
	}

	rowSizes.push_back(currentRowSize);
	for (size_t i = 0; i < rowSizes.size(); i++)
	{
		rowsSize += rowSizes[i];
	}

	if (m_XAlign == AlignHorizontally::XCENTER)
	{
		position.x -= rowsSize.x * 0.5f;
	}
	if (m_YAlign == AlignVertically::YCENTER)
	{
		position.y += rowsSize.y * 0.5f;
		lowestYOffset *= 0.5f;
	}
	position.y += lowestYOffset;

	float characterXOffset = 0;
	size_t lineCounter = 0;
	for (size_t i = 0; i < charactersSize; i++)
	{
		auto newLinesIter = std::find(newLines.begin(), newLines.end(), i);
		if (newLinesIter != newLines.end())
		{
			position.y -= m_Theme.m_SpaceBetweenLines + rowSizes[lineCounter].y;
			characterXOffset = 0;
			newLines.erase(newLinesIter);
			lineCounter++;
		}
		Character* character = characters[i];
		character->m_Color = m_Theme.m_TextColor;
		character->m_Position.x = position.x + characterXOffset;
		character->m_Position.y = position.y;
		characterXOffset += character->m_XAdvance * character->m_Scale.x;

		Batch::GetInstance().Submit(character);
	}
	m_LastUIPropertiesOnPanel.m_Size = rowsSize;
	m_LastUIPropertiesOnPanel.m_Position = position;
	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset += glm::abs(startPos.y - position.y) + rowSizes.back().y + m_RelatedPanelProperties.m_Padding.y;
	}
	va_end(argptr);
}

bool Gui::Button(const std::wstring& label, glm::vec2 pos, Texture* texture, glm::vec2 size)
{
	ApplyRelatedToPanelProperties(&pos.x, &pos.y);
	if (Utils::IsEqual(size, { 0.0f, 0.0f })) size = m_Theme.m_ButtonSize;
	float startOffset = m_RelatedPanelProperties.m_YOffset;
	glm::vec2 startPos = pos;
	glm::vec2 halfSize = { size.x * 0.5f, -size.y * 0.5f };
	pos += halfSize;
	if (m_XAlign == AlignHorizontally::XCENTER)
	{
		pos.x += m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x - size.x * 0.5f;
	}
	if (m_YAlign == AlignVertically::YCENTER)
	{
		pos.y -= m_RelatedPanelProperties.m_Size.y * 0.5f - m_RelatedPanelProperties.m_Margin.y - size.y * 0.5f;
	}

	float vertices[8];
	CreateRect(vertices, pos, halfSize);

	glm::vec4 color = m_Theme.m_ButtonColor;
	std::wstring uniqueId = label + std::to_wstring(pos.x + pos.y);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());
	bool isPressed = ButtonBehavior(uniqueId, vertices, color, m_IsCurrentPanelHovered);

	Batch::GetInstance().Submit(vertices, color, pos, size, texture);

	if (m_IsRelatedToPanel)
	{
		startOffset += glm::abs(startPos.y - (pos.y + size.y * 0.5f)) + size.y + m_RelatedPanelProperties.m_Padding.y;
	}

	if (label.length() > 0) 
	{
		m_XAlign = AlignHorizontally::XCENTER;
		m_YAlign = AlignVertically::YCENTER;

		//because it is applied in text, so it is needed to be compensated
		if (m_RelatedPanelProperties.m_AutoAllignment)
		{
			pos.y += m_RelatedPanelProperties.m_YOffset;
		}
		pos.x -= m_RelatedPanelProperties.m_Position.x + m_RelatedPanelProperties.m_Margin.x;
		pos.y -= m_RelatedPanelProperties.m_Position.y - m_RelatedPanelProperties.m_Margin.y;

		Text(label, pos);
		
		m_YAlign = AlignVertically::BOTTOM;
		m_XAlign = AlignHorizontally::LEFT;
	}
	else
	{
		m_LastUIPropertiesOnPanel = UIProperties();
	}
	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset = startOffset;
	}

	m_LastUIPropertiesOnPanel.m_Size = size;
	m_LastUIPropertiesOnPanel.m_YOffset = m_RelatedPanelProperties.m_YOffset;

	return isPressed;
}

void Gui::RoundCorners(Edge edge)
{
	m_Theme.m_PreviousRoundedCorners = m_Theme.m_RoundedCorners;
	switch (edge)
	{
		case Gui::Edge::TOPLEFT:
		{
			m_Theme.m_RoundedCorners[0] = true;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		case Gui::Edge::BOTTOMLEFT:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = true;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		case Gui::Edge::BOTTOMRIGHT:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = true;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		case Gui::Edge::TOPRIGHT:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = true;
			break;
		}
		case Gui::Edge::TOP:
		{
			m_Theme.m_RoundedCorners[0] = true;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = true;
			break;
		}
		case Gui::Edge::LEFT:
		{
			m_Theme.m_RoundedCorners[0] = true;
			m_Theme.m_RoundedCorners[1] = true;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		case Gui::Edge::BOTTOM:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = true;
			m_Theme.m_RoundedCorners[2] = true;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		case Gui::Edge::RIGHT:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = true;
			m_Theme.m_RoundedCorners[3] = true;
			break;
		}
		case Gui::Edge::ALL:
		{
			m_Theme.m_RoundedCorners[0] = true;
			m_Theme.m_RoundedCorners[1] = true;
			m_Theme.m_RoundedCorners[2] = true;
			m_Theme.m_RoundedCorners[3] = true;
			break;
		}
		case Gui::Edge::NONE:
		{
			m_Theme.m_RoundedCorners[0] = false;
			m_Theme.m_RoundedCorners[1] = false;
			m_Theme.m_RoundedCorners[2] = false;
			m_Theme.m_RoundedCorners[3] = false;
			break;
		}
		default:
		{
			break;
		}
	}
}

bool Gui::ButtonBehavior(const std::wstring& uniqueId, float vertices[8], glm::vec4& color, bool isCurrentPanelHovered)
{
	bool isPressed = false;

	if (m_IsButtonClickable && m_CurrentDown == L"")
	{
		bool isHovered = IsRectHovered(vertices);
		if (isHovered)
		{
			if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1))
			{
				m_CurrentDown = uniqueId;
			}
			else
			{
				color = m_Theme.m_HoveredColor;
			}
		}
	}

	if (uniqueId == m_CurrentDown)
	{
		color = m_Theme.m_PressedColor;
		bool buttonAction = false;
		bool isHovered = false;
		switch (m_Theme.m_ButtonAction)
		{
			case ButtonAction::DOWN:
			{
				buttonAction = Input::IsMouseDown(Keycode::MOUSE_BUTTON_1);
				//An assumption that if I wanna "drag" a button I don't have to intersect it
				isHovered = true;
				break;
			}
			case ButtonAction::PRESSED:
			{
				buttonAction = Input::IsMousePressed(Keycode::MOUSE_BUTTON_1);
				isHovered = IsRectHovered(vertices);
				break;
			}
			case ButtonAction::RELEASED:
			{
				buttonAction = Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1);
				isHovered = IsRectHovered(vertices);
				break;
			}
			default:
				break;
		}
		if (buttonAction && isHovered && isCurrentPanelHovered)
		{
			isPressed = true;
		}
	}

	return isPressed;
}

void Gui::BeginPanel(const std::wstring& label, glm::vec2 pos, glm::vec2 size, Texture* texture)
{
	m_CurrentPanelPosition = pos;
	m_CurrentPanelSize = size;
	m_RelatedPanelProperties.m_Position = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Margin = glm::vec2(15.0f);
	m_RelatedPanelProperties.m_Padding = glm::vec2(15.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
	m_RelatedPanelProperties.m_AutoAllignment = true;
	m_IsRelatedToPanel = true;
	m_IsRelatedToMainPanel = true;
	m_BackUpUIProperties = m_RelatedPanelProperties;
	for (size_t i = 0; i < 8; i++)
	{
		m_BackUpCurrentPanelCoords[i] = m_CurrentPanelCoords[i];
	}
	Panel(label, pos, size, texture);
}

void Gui::Panel(const std::wstring& label, glm::vec2 pos, glm::vec2 size, Texture* texture)
{
	glm::vec2 halfSize = { size.x * 0.5f, -size.y * 0.5f };
	CreateRect(m_CurrentPanelCoords, pos, halfSize);

	glm::vec4 color = m_Theme.m_PanelColor;
	bool isHovered = IsRectHovered(m_CurrentPanelCoords);
	m_IsCurrentPanelHovered = isHovered;
	if (isHovered)
	{
		if (m_Theme.m_ChangeColorWhenPanelIsHovered && m_CurrentDown == L"")
		{
			color = m_Theme.m_HoveredColor;
		}
	}

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_Position = glm::vec2(pos.x - size.x * 0.5f, pos.y + size.y * 0.5f);
		m_RelatedPanelProperties.m_Size = size;
	}

	Batch::GetInstance().Submit(m_CurrentPanelCoords, color, pos, size, texture);

	if (label.size() > 0 && label.find(L"##") == std::string::npos)
	{
		glm::vec2 tempMargin = m_RelatedPanelProperties.m_Margin;
		glm::vec2 tempPadding = m_RelatedPanelProperties.m_Padding;
		m_RelatedPanelProperties.m_Margin = glm::vec3(5);
		m_RelatedPanelProperties.m_Padding = glm::vec2(0);
		pos.x = 0;
		if (m_XAlign == AlignHorizontally::XCENTER)
		{
			pos.x = m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x;
		}

		Text(label, glm::vec2(pos.x, 0));

		m_RelatedPanelProperties.m_Margin = tempMargin;
		m_RelatedPanelProperties.m_Padding = tempPadding;
	}
}

void Gui::EndPanel()
{
	m_RelatedPanelProperties = m_BackUpUIProperties;
	for (size_t i = 0; i < 8; i++)
	{
		m_CurrentPanelCoords[i] = m_BackUpCurrentPanelCoords[i];
	}
	m_RelatedPanelProperties.m_Position = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Margin = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
	m_RelatedPanelProperties.m_AutoAllignment = false;
	m_IsRelatedToPanel = false;
	m_IsRelatedToMainPanel = false;
}

void Gui::Bar(glm::vec2 position, float value, float maxValue, glm::vec2 size)
{
	if (Utils::IsEqual(size, { 0.0f, 0.0f })) size = m_Theme.m_BarSize;
	ApplyRelatedToPanelProperties(&position.x, &position.y);
	glm::vec2 halfSize = { size.x * 0.5f, -size.y * 0.5f };
	position += halfSize;
	std::wstring uniqueId = std::to_wstring(position.x + position.y);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());

	float vertices[8];
	CreateRect(vertices, position, halfSize);

	Batch::GetInstance().Submit(vertices, m_Theme.m_BarBackGroundColor, position, size);

	float currentWidth = size.x * (value / maxValue);
	float difference = size.x - currentWidth;

	position.x -= difference * 0.5f;
	halfSize.x = currentWidth * 0.5f;
	CreateRect(vertices, position, halfSize);

	size = { currentWidth, size.y };
	Batch::GetInstance().Submit(vertices, m_Theme.m_BarColor, position, size);

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset += size.y + m_RelatedPanelProperties.m_Padding.y;
	}
}

bool Gui::CheckBox(const std::wstring& label, glm::vec2 position, bool* value, glm::vec2 size)
{
	if (Utils::IsEqual(size, { 0.0f, 0.0f })) size = m_Theme.m_CheckBoxSize;
	float startOffset = m_RelatedPanelProperties.m_YOffset;
	Texture* texture = m_CheckBoxTextureFalse;
	glm::vec4 color = { 1.0f, 0.0f, 0.0f, 1.0f };
	bool pressed = false;
	if (*value) 
	{
		color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		texture = m_CheckBoxTextureTrue;
	}

	glm::vec4 tempButtonColor = m_Theme.m_ButtonColor;
	m_Theme.m_ButtonColor = color;

	if (Button(L"##Button", position, texture, size))
	{
		*value = !*value;
		pressed = true;
	}

	m_Theme.m_ButtonColor = tempButtonColor;
	m_RelatedPanelProperties.m_YOffset -= size.y + m_RelatedPanelProperties.m_Padding.y;

	AlignVertically tempAlignY = m_YAlign;
	AlignHorizontally tempAlignX = m_XAlign;
	m_YAlign = AlignVertically::YCENTER;
	m_XAlign = AlignHorizontally::LEFT;

	Text(label, glm::vec2(position.x + size.x, position.y - size.y * 0.5f));

	m_YAlign = tempAlignY;
	m_XAlign = tempAlignX;

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset = startOffset + size.y + m_RelatedPanelProperties.m_Padding.y;
	}

	return pressed;
}

bool Gui::SliderFloat(const std::wstring& label, glm::vec2 position, glm::vec2 limits, float* value, glm::vec2 size)
{
	if (Utils::IsEqual(size, { 0.0f, 0.0f })) size = m_Theme.m_SliderSize;
	ApplyRelatedToPanelProperties(&position.x, &position.y);
	glm::vec2 halfSize = { size.x * 0.5f, -size.y * 0.5f };
	position += halfSize;
	
	std::wstring uniqueId = label + std::to_wstring(position.x + position.y);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());
	
	float vertices[8];
	CreateRect(vertices, position, halfSize);

	Batch::GetInstance().Submit(vertices, m_Theme.m_SliderColor, position, size);

	float maxValue = 0;
	float min = limits.x;
	float max = limits.y;
	if (min < 0 && max < 0) maxValue = abs(min - max);
	else maxValue = max - min;

	if (IsRectHovered(vertices) && m_CurrentDown == L"")
	{
		if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1))
		{
			m_CurrentDown = uniqueId;
		}
	}

	bool isActive = false;
	if (m_CurrentDown == uniqueId)
	{
		isActive = true;
		glm::vec2 mPos = Viewport::GetInstance().GetUIMousePosition();
		float _value = (mPos.x - position.x - size.x * 0.5f);
		*value = ((1 + ((_value) / (size.x))) * maxValue) + min;
		if (Input::IsKeyDown(Keycode::KEY_LEFT_SHIFT)) *value = glm::round(*value);
		if (*value > max) *value = max;
		else if (*value < min) *value = min;
	}

	float percent = ((*value - min)) / (max - min);
	float xOffset = percent * size.x;
	glm::vec2 sliderPos = glm::vec2(position.x - size.x * 0.5f + xOffset, position.y);
	CreateRect(vertices, sliderPos, halfSize * m_Theme.m_SliderRatioToRect);

	Batch::GetInstance().Submit(vertices, m_Theme.m_SelectedColor, sliderPos, size * m_Theme.m_SliderRatioToRect);

	float startYOffset = m_RelatedPanelProperties.m_YOffset;
	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	
	Text(L"##SliderFloat" + uniqueId + L" %f", halfSize, *value);
	
	m_RelatedPanelProperties.m_YOffset = startYOffset;
	m_XAlign = LEFT;

	Text(label, glm::vec2(size.x + m_RelatedPanelProperties.m_Padding.x, halfSize.y));
	
	m_YAlign = TOP;

	m_RelatedPanelProperties.m_YOffset = startYOffset += size.y + m_RelatedPanelProperties.m_Padding.y;
	
	return isActive;
}

void Gui::InputInt(const std::wstring& label, int64_t* value, float x, float y, float width, float height, glm::vec4 panelColor)
{
	auto iter = m_Active.find(label);
	if (iter == m_Active.end()) {
		m_Active.insert(std::make_pair(label, false));
		iter = m_Active.find(label);
	}

	auto iter1 = m_NegativeInputs.find(label);
	if (iter1 == m_NegativeInputs.end()) {
		m_NegativeInputs.insert(std::make_pair(label, false));
		iter1 = m_NegativeInputs.find(label);
	}

	glm::dvec2 mousePos = Viewport::GetInstance().GetUIMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);

	x += width * 0.5f;
	y -= height * 0.5f;
	std::wstring uniqueId = std::to_wstring(x);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoords[i];

	Panel(L"##InputInt" + uniqueId, glm::vec2(x, y), glm::vec2(width, height));

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (IsRectHovered(m_CurrentPanelCoords))
		{
			iter->second = true;
			m_IsInteracting = true;
		}
		else 
		{
			iter->second = false;
			m_IsInteracting = false;
		}
	}

	m_RelatedPanelProperties = temp;

	if (iter->second == true)
	{
		std::string stringValue;
		std::ostringstream Q;
		Q << std::setprecision(6) << std::setw(20) << *value;
		stringValue = Q.str();

		int key = -1;

		if (Input::IsKeyPressed(Keycode::KEY_0)) key = Keycode::KEY_0;
		else if (Input::IsKeyPressed(Keycode::KEY_1)) key = Keycode::KEY_1;
		else if (Input::IsKeyPressed(Keycode::KEY_2)) key = Keycode::KEY_2;
		else if (Input::IsKeyPressed(Keycode::KEY_3)) key = Keycode::KEY_3;
		else if (Input::IsKeyPressed(Keycode::KEY_4)) key = Keycode::KEY_4;
		else if (Input::IsKeyPressed(Keycode::KEY_5)) key = Keycode::KEY_5;
		else if (Input::IsKeyPressed(Keycode::KEY_6)) key = Keycode::KEY_6;
		else if (Input::IsKeyPressed(Keycode::KEY_7)) key = Keycode::KEY_7;
		else if (Input::IsKeyPressed(Keycode::KEY_8)) key = Keycode::KEY_8;
		else if (Input::IsKeyPressed(Keycode::KEY_9)) key = Keycode::KEY_9;
		
		if (key != -1)
		{
			stringValue += std::to_string(key - 48);
			std::istringstream(stringValue) >> *value;
		}

		//if (Input::IsKeyPressed(Keycode::MINUS))
		//{
		//	stringValue = "-0";
		//	std::istringstream(stringValue) >> *value;
		//	iter1->second = !iter1->second;
		//}

		if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE))
		{
			if (stringValue.empty() == false) stringValue.pop_back();
			if (*stringValue.rbegin() == 32) *value = 0;
			else std::istringstream(stringValue) >> *value;
		}

		if (iter1->second) *value *= *value < 0 ? 1 : -1;
	}

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	Text(L"%d", glm::vec2(width * 0.5f, -height * 0.5f), *value);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, glm::vec2(width + 10, -height * 0.5f));
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastUIPropertiesOnPanel.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoords[i] = tempCoords[i];
}

void Gui::InputDouble(const std::wstring& label, double* value, float x, float y, float width, float height, glm::vec4 panelColor)
{
	auto iter = m_Active.find(label);
	if (iter == m_Active.end()) {
		m_Active.insert(std::make_pair(label, false));
		iter = m_Active.find(label);
	}

	auto iter1 = m_NegativeInputs.find(label);
	if (iter1 == m_NegativeInputs.end()) {
		m_NegativeInputs.insert(std::make_pair(label, false));
		iter1 = m_NegativeInputs.find(label);
	}

	glm::dvec2 mousePos = Viewport::GetInstance().GetUIMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	std::wstring uniqueId = std::to_wstring(x);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoords[i];
	Panel(L"##InputDouble" + uniqueId, glm::vec2(x, y), glm::vec2(width, height));

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (IsRectHovered(m_CurrentPanelCoords))
		{
			iter->second = true;
			m_IsInteracting = true;
		}
		else
		{
			iter->second = false;
			m_IsInteracting = false;
		}
	}

	m_RelatedPanelProperties = temp;

	std::ostringstream Q;
	Q << std::setprecision(6) << std::setw(20) << *value;

	auto iter2 = m_DoubleInput.find(label);
	if (iter2 == m_DoubleInput.end()) {
		m_DoubleInput.insert(std::make_pair(label, Q.str()));
		iter2 = m_DoubleInput.find(label);
	}

	iter2->second.erase(std::remove(iter2->second.begin(), iter2->second.end(), 32), iter2->second.end());
	std::string stringValue = iter2->second;

	size_t index = stringValue.find('.');

	if (iter->second == true)
	{
		int key = -1;

		if (Input::IsKeyPressed(Keycode::KEY_0)) key = Keycode::KEY_0;
		else if (Input::IsKeyPressed(Keycode::KEY_1)) key = Keycode::KEY_1;
		else if (Input::IsKeyPressed(Keycode::KEY_2)) key = Keycode::KEY_2;
		else if (Input::IsKeyPressed(Keycode::KEY_3)) key = Keycode::KEY_3;
		else if (Input::IsKeyPressed(Keycode::KEY_4)) key = Keycode::KEY_4;
		else if (Input::IsKeyPressed(Keycode::KEY_5)) key = Keycode::KEY_5;
		else if (Input::IsKeyPressed(Keycode::KEY_6)) key = Keycode::KEY_6;
		else if (Input::IsKeyPressed(Keycode::KEY_7)) key = Keycode::KEY_7;
		else if (Input::IsKeyPressed(Keycode::KEY_8)) key = Keycode::KEY_8;
		else if (Input::IsKeyPressed(Keycode::KEY_9)) key = Keycode::KEY_9;

		if (key != -1)
		{
			if (stringValue.size() == 1 && stringValue[0] == '0') stringValue.pop_back();
			if (stringValue.size() < 5) 
			{
				stringValue += std::to_string(key - 48);
				std::istringstream(stringValue) >> *value;
			}
		}

		//if (Input::IsKeyPressed(Keycode::MINUS))
		//{
		//	stringValue = "-0";
		//	std::istringstream(stringValue) >> *value;
		//	iter1->second = !iter1->second;
		//}
		if (Input::IsKeyPressed(Keycode::PERIOD))
		{
			if (index == std::string::npos)
			{
				stringValue += ".";
				std::istringstream(stringValue) >> *value;
			}
		}
		else if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE))
		{
			if (stringValue.empty() == false) stringValue.pop_back();
			if (stringValue.empty() == true) stringValue += '0';
			std::istringstream(stringValue) >> *value;
		}

		if (iter1->second) *value *= *value < 0 ? 1 : -1;
		iter2->second = stringValue;
	}

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	//int m_Theme.m_CharsAfterComma = index == std::string::npos ? 0 : stringValue.size() - index;
	Text(L"%s", glm::vec2(width * 0.5f, -height * 0.5f), stringValue);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, glm::vec2(width + 10, -height * 0.5f));
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastUIPropertiesOnPanel.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoords[i] = tempCoords[i];
}

void Gui::Image(glm::vec2 pos, Texture* texture, glm::vec2 size)
{
	if (Utils::IsEqual(size, { 0.0f, 0.0f })) size = m_Theme.m_ImageSize;
	ApplyRelatedToPanelProperties(&pos.x, &pos.y);
	if (m_IsRelatedToPanel)
	{
		m_LastUIPropertiesOnPanel.m_YOffset = m_RelatedPanelProperties.m_YOffset;
		m_LastUIPropertiesOnPanel.m_Size = size;
		m_LastUIPropertiesOnPanel.m_Position = pos;
	}
	glm::vec2 halfSize = { size.x * 0.5f, -size.y * 0.5f };
	pos += halfSize;

	float vertices[8];
	CreateRect(vertices, pos, halfSize);

	Batch::GetInstance().Submit(vertices, m_Theme.m_ImageColor, pos, size, texture);

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset += size.y + m_RelatedPanelProperties.m_Padding.y;
	}
}

bool Gui::CollapsingHeader(const std::wstring& label, glm::vec2 pos, float height)
{
	ApplyRelatedToPanelProperties(&pos.x, &pos.y);
	float startOffset = m_RelatedPanelProperties.m_YOffset;
	glm::vec2 startPos = pos;
	float width = m_RelatedPanelProperties.m_Size.x;
	glm::vec2 halfSize = { width * 0.5f, -height * 0.5f };
	pos += halfSize;
	pos.x -= m_RelatedPanelProperties.m_Margin.x;
	
	float vertices[8];
	CreateRect(vertices, pos, halfSize);

	std::wstring uniqueId = label + std::to_wstring(pos.x + pos.y);
	uniqueId.erase(remove_if(uniqueId.begin(), uniqueId.end(), isspace), uniqueId.end());
	glm::vec4 color = m_Theme.m_CollapsingHeaderColor;
	bool isPressed = ButtonBehavior(uniqueId, vertices, color, true);

	Batch::GetInstance().Submit(vertices, color, pos, glm::vec2(width, height), nullptr);

	bool isOpened = false;
	auto activeIter = m_Active.find(uniqueId);
	if (activeIter != m_Active.end())
	{
		if (isPressed)
		{
			activeIter->second = !activeIter->second;
		}
		isOpened = activeIter->second;
	}
	else
	{
		isOpened = isPressed;
		m_Active.insert(std::make_pair(uniqueId, isPressed));
	}

	if(isOpened) Image(glm::vec2(0.0f, -height * 0.25f), m_TriangleDownTexture , glm::vec2(0.5f * height, 0.5f * height));
	else		 Image(glm::vec2(0.0f, -height * 0.25f), m_TriangleRightTexture, glm::vec2(0.5f * height, 0.5f * height));

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	}

	AlignVertically tempAlign = m_YAlign;
	m_YAlign = AlignVertically::YCENTER;
	Text(label, glm::vec2(height, -height * 0.5f));
	m_YAlign = tempAlign;

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_YOffset = startOffset + height + m_RelatedPanelProperties.m_Padding.y;
	}
	return isOpened;
}

void Gui::ListBox(const std::wstring& label, glm::vec2 position, std::vector<std::wstring> items, int& selectedItem, glm::vec2 size)
{
	glm::vec2 buttonSize = Utils::IsEqual(size, { 0.0f, 0.0f }) ? m_Theme.m_ListButtonSize : size;
	glm::vec2 tempPadding = m_RelatedPanelProperties.m_Padding;
	m_RelatedPanelProperties.m_Padding = m_Theme.m_ListBoxPadding;
	
	Text(label, position);
	
	ApplyRelatedToPanelProperties(&position.x, &position.y);
	size_t maxItems = items.size() < m_Theme.m_MaxItemInListBox ? items.size() : m_Theme.m_MaxItemInListBox;
	float fullHeight = buttonSize.y * items.size() + (items.size() - 1) * m_Theme.m_ListBoxPadding.y;
	float actualHeight = buttonSize.y * maxItems + (maxItems - 1) * m_Theme.m_ListBoxPadding.y;

	glm::vec2 halfSize = { (buttonSize.x + m_Theme.m_ListBoxPadding.x) * 0.5f, -actualHeight * 0.5f };
	float vertices[8];
	CreateRect(vertices, position + halfSize, halfSize);

	bool tempIsCurrentPanelHovered = m_IsCurrentPanelHovered;
	bool isHovered = IsRectHovered(vertices);
	m_IsCurrentPanelHovered = isHovered;

	RoundCorners(Edge::RIGHT);

	float YOffset = ScrollBar(L"##" + label + L"ScrollBar", position, halfSize, actualHeight, fullHeight, isHovered);

	RoundCorners(Edge::LEFT);

	Batch::GetInstance().Submit(
		vertices,
		m_Theme.m_ListBoxColor,
		position + halfSize,
		glm::vec2(buttonSize.x + m_Theme.m_ListBoxPadding.x, actualHeight),
		nullptr
	);

	RoundCorners(Edge::ALL);
	glm::vec2 tempCurrentPanelPos = m_CurrentPanelPosition;
	glm::vec2 tempCcurentPanelSize = m_CurrentPanelSize;
	m_CurrentPanelSize = { buttonSize.x, actualHeight };
	m_CurrentPanelPosition = { position.x + m_CurrentPanelSize.x * 0.5, position.y - m_CurrentPanelSize.y * 0.5f };
	glm::vec4 tempButtonColor = m_Theme.m_ButtonColor;
	glm::vec4 tempHoveredColor = m_Theme.m_HoveredColor;
	glm::vec4 tempPressedColor = m_Theme.m_PressedColor;
	if (!isHovered)
	{
		m_IsButtonClickable = false;
	}
	for (size_t i = 0; i < items.size(); i++)
	{
		if (selectedItem == i)
		{
			m_Theme.m_ButtonColor = m_Theme.m_SelectedColor;
			m_Theme.m_HoveredColor = m_Theme.m_HoveredSelectedColor;
			m_Theme.m_HoveredColor = m_Theme.m_HoveredSelectedColor;
			m_Theme.m_PressedColor = m_Theme.m_PressedSelectedColor;
		}
		
		glm::vec2 buttonPos = { 0.0f, position.y + YOffset - (i + 1) * buttonSize.y - (i) * m_Theme.m_ListBoxPadding.y };
		bool isButtonInsideOfRect = buttonPos.y < position.y && buttonPos.y + buttonSize.y > position.y - actualHeight;
		if (isButtonInsideOfRect)
		{
			if (Button(items[i], glm::vec2(0.0f, YOffset), nullptr, buttonSize))
			{
				selectedItem = i;
			}
		}
		else
		{
			m_RelatedPanelProperties.m_YOffset += m_Theme.m_ListBoxPadding.y;
			m_RelatedPanelProperties.m_YOffset += buttonSize.y;
		}
		m_Theme.m_ButtonColor = tempButtonColor;
		m_Theme.m_HoveredColor = tempHoveredColor;
		m_Theme.m_PressedColor = tempPressedColor;
	}
	m_IsButtonClickable = true;
	m_RelatedPanelProperties.m_YOffset -= m_Theme.m_ListBoxPadding.y - tempPadding.y + (fullHeight - actualHeight);
	m_RelatedPanelProperties.m_Padding = tempPadding;
	m_CurrentPanelPosition = tempCurrentPanelPos;
	m_CurrentPanelSize = tempCcurentPanelSize;
	m_IsCurrentPanelHovered = tempIsCurrentPanelHovered;
}

bool Gui::IsAnyPanelHovered() const
{
	return m_IsAnyPanelHovered;
}

bool Gui::IsRectHovered(const float vertices[8])
{
	glm::dvec2 mousePos = Viewport::GetInstance().GetUIMousePosition();
	bool isHovered = (mousePos.x > vertices[0] && mousePos.x < vertices[2] && mousePos.y > vertices[1] && mousePos.y < vertices[7]);
	if (isHovered == true) m_IsAnyPanelHovered = true;
	return isHovered;
}

void Gui::Begin()
{
	m_IsAnyPanelHovered = false;
	Batch::GetInstance().BeginUI();
}

void Gui::End()
{
	Batch::GetInstance().EndUI();
	if (!Input::IsMouseDown(Keycode::MOUSE_BUTTON_1))
	{
		m_CurrentDown = L"";
	}
	else
	{
		if (m_CurrentDown == L"")
		{
			m_CurrentDown = L"Empty";
		}
	}
}

void Gui::SetFont(const Font& font)
{
	m_Font = &font;
}

const Font& Gui::GetFont() const
{
	return *m_Font;
}

const Font& Gui::GetStandardFont(StandardFonts standardFont) const
{
	return *m_StandardFonts[standardFont];
}

const float* Gui::GetCurrentPanelRect() const
{
	return m_CurrentPanelCoords;
}

void Gui::SameLine()
{
	m_RelatedPanelProperties.m_YOffset = m_LastUIPropertiesOnPanel.m_YOffset;
}

void Gui::LoadStandartFonts()
{
	Font* font = new Font();
	font->LoadFont("Source/Fonts/calibri.txt", "Source/Fonts/calibri.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	font = new Font();
	font->LoadFont("Source/fonts/arial.txt", "Source/fonts/arial.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	font = new Font();
	font->LoadFont("Source/fonts/harrington.txt", "Source/fonts/harrington.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	font = new Font();
	font->LoadFont("Source/Fonts/Peak.txt", "Source/Fonts/Peak.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	font = new Font();
	font->LoadFont("Source/Fonts/playball.txt", "Source/Fonts/playball.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	font = new Font();
	font->LoadFont("Source/Fonts/segoe.txt", "Source/Fonts/segoe.png");
	font->LoadCharacters();
	m_StandardFonts.push_back(font);

	SetFont(*m_StandardFonts.front());
}

void Gui::ApplyRelatedToPanelProperties(float* x, float* y)
{
	if (!m_IsRelatedToPanel) return;
	*x += m_RelatedPanelProperties.m_Position.x + m_RelatedPanelProperties.m_Margin.x;
	*y += m_RelatedPanelProperties.m_Position.y - m_RelatedPanelProperties.m_Margin.y;
	if (m_RelatedPanelProperties.m_AutoAllignment)
	{
		*y -= m_RelatedPanelProperties.m_YOffset; //NEED to test, not sure that it will work with all UI elements
	}
}

Character* Gui::FindCharInFont(wchar_t character)
{
	auto iter = m_Font->m_Characters.find(character);
	if (iter != m_Font->m_Characters.end())
	{
		return iter->second;
	}
	return m_Font->m_Characters.find(L'?')->second;
}

float Gui::ScrollBar(const std::wstring& label, glm::vec2 position, glm::vec2 size, float actualHeight, float fullHeight, bool isHovered)
{
	if (fullHeight == actualHeight) return 0.0f;

	float sizeOfRelatedRect = size.x;
	size.x *= m_Theme.m_ScrollBarWidthRelativeToRect;
	float YOffset = 0.0f;
	auto scrollBarIter = m_ScrollBars.find(label);
	if (scrollBarIter == m_ScrollBars.end())
	{
		m_ScrollBars.insert(std::make_pair(label, YOffset));
		scrollBarIter = m_ScrollBars.find(label);
	}
	else
	{
		YOffset = scrollBarIter->second;
	}

	float vertices[8];
	CreateRect(
		vertices,
		glm::vec2(position.x + size.x + sizeOfRelatedRect * 2.0f, position.y + size.y),
		size
	);
	
	float ratio = actualHeight / fullHeight;

	Batch::GetInstance().Submit(
		vertices,
		m_Theme.m_ScrollBarCollor,
		glm::vec2(position.x + size.x + sizeOfRelatedRect * 2.0f, position.y + size.y),
		glm::vec2(size.x * 2.0f, actualHeight),
		nullptr
	);

	bool isHoveredScrollBar = IsRectHovered(vertices);

	CreateRect(vertices,
		glm::vec2(position.x + size.x + sizeOfRelatedRect * 2.0f, ratio * (size.y - YOffset) + position.y),
		glm::vec2(size.x, ratio * size.y)
	);

	glm::vec4 color = m_Theme.m_ScrollBarIndicatorCollor;
	ButtonAction tempButtonAction = m_Theme.m_ButtonAction;
	m_Theme.m_ButtonAction = ButtonAction::DOWN;

	if (isHovered || isHoveredScrollBar)
	{
		Window& window = Window::GetInstance();
		scrollBarIter->second -= window.GetScrollOffset().y * m_Theme.m_ScrollScale;
	}
	glm::vec4 tempHoveredColor = m_Theme.m_HoveredColor;
	glm::vec4 tempPressedColor = m_Theme.m_PressedColor;
	m_Theme.m_HoveredColor = m_Theme.m_HoveredSelectedColor;
	m_Theme.m_PressedColor = m_Theme.m_PressedSelectedColor;
	if (ButtonBehavior(label + L"Slider", vertices, color))
	{
		//0.666f fits kind of good because if we look at vertices y component then we see that we multiply ratio by 3 components,
		//so we need to cast YOffset to new coordinates, so our ratio is 2/3 => 0.666f
		scrollBarIter->second -= (float)(Viewport::GetInstance().GetDragDelta().y) / (ratio * 0.666f);
	}
	else if (isHoveredScrollBar && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		int sign = Viewport::GetInstance().GetUIMousePosition().y < vertices[1] ? -1 : 1;
		scrollBarIter->second -= sign * (m_Theme.m_ScrollingByClickingOffset * fullHeight + ratio * size.y);
	}
	m_Theme.m_HoveredColor = tempHoveredColor;
	m_Theme.m_PressedColor = tempPressedColor;
	scrollBarIter->second = glm::clamp(scrollBarIter->second, 0.0f, fullHeight - actualHeight);
	YOffset = scrollBarIter->second;
	
	m_Theme.m_ButtonAction = tempButtonAction;

	//if comment this section then rendering of this scrollbar will be delayed for 1 frame
	vertices[1] = ratio * (2.0f * size.y - YOffset) + position.y;
	vertices[3] = ratio * (2.0f * size.y - YOffset) + position.y;
	vertices[5] = ratio * (/*-size.y + size.y*/ - YOffset) + position.y;
	vertices[7] = ratio * (/*-size.y + size.y*/ - YOffset) + position.y;

	Batch::GetInstance().Submit(
		vertices,
		color,
		glm::vec2(position.x + size.x + sizeOfRelatedRect * 2.0f, ratio * (size.y - YOffset) + position.y),
		glm::vec2(size.x * 2.0f, ratio * -size.y * 2.0f),
		nullptr
	);

	return YOffset;
}

void Gui::CreateRect(float vertices[8], glm::vec2 position, glm::vec2 size)
{
	vertices[0] = -size.x + position.x; vertices[1] =  size.y + position.y;
	vertices[2] =  size.x + position.x; vertices[3] =  size.y + position.y;
	vertices[4] =  size.x + position.x; vertices[5] = -size.y + position.y;
	vertices[6] = -size.x + position.x; vertices[7] = -size.y + position.y;
}

void Gui::RecalculateProjectionMatrix()
{
	m_Aspect = Viewport::GetInstance().GetAspect();
	float halfRelativeHeight = m_RelativeHeight * 0.5f;
	m_ViewProjection = glm::ortho(-halfRelativeHeight * m_Aspect, halfRelativeHeight * m_Aspect,
		-halfRelativeHeight, halfRelativeHeight, -1.0f, 1.0f);
	/*glm::vec2 size = Viewport::GetInstance().GetSize();
	size *= 0.5f;
	m_ViewProjection = glm::ortho(-size.x, size.x,
		-size.y, size.y, -1.0f, 1.0f);*/
}

void Gui::ShutDown()
{
	for (size_t i = 0; i < m_StandardFonts.size(); i++)
	{
		delete m_StandardFonts[i];
	}
	m_StandardFonts.clear();
}

glm::vec2 Gui::GetFromWorldToScreenSpace(const glm::vec2 position)
{
	glm::vec2 screenSpacePosition;/* = glm::vec2(
		position.x / (Viewport::GetInstance().() * Window::GetInstance().GetCamera().m_ZoomScale),
		position.y / Window::GetInstance().GetCamera().m_ZoomScale
	);
	screenSpacePosition *= glm::vec2(Window::GetInstance().GetSize()) * 0.5f;*/
	return screenSpacePosition;
}