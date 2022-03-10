#pragma once

#include "../Core/Core.h"
#include "Character.h"
#include <stdarg.h>

#include <map>

namespace Pengine {

	class PENGINE_API Gui {
	public:

		enum ButtonAction;

		struct Theme
		{
		public:

			glm::vec4 m_TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec4 m_PressedColor = { 0.4f, 0.4f, 0.4f, 1.0f };
			glm::vec4 m_HoveredColor = { 0.6f, 0.6f, 0.6f, 1.0f };
			glm::vec4 m_ButtonColor = { 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 m_SliderColor = { 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 m_PanelColor = { 0.7f, 0.7f, 0.7f, 0.4f };
			glm::vec4 m_ImageColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			glm::vec4 m_BarColor = { 1.0f, 0.0f, 0.0f, 1.0f };
			glm::vec4 m_BarBackGroundColor = { 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 m_CollapsingHeaderColor = { 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 m_SelectedColor = { 1.0f, 0.0f, 0.0f, 1.0f };
			glm::vec4 m_HoveredSelectedColor = glm::vec4(glm::vec3(m_SelectedColor) * 0.8f, 1.0f);
			glm::vec4 m_PressedSelectedColor = glm::vec4(glm::vec3(m_HoveredSelectedColor) * 0.8f, 1.0f);
			glm::vec4 m_ListBoxColor = { 0.7f, 0.7f, 0.7f, 0.6f };
			glm::vec4 m_ScrollBarCollor = { 0.7f, 0.7f, 0.7f, 1.0f };
			glm::vec4 m_ScrollBarIndicatorCollor = { 1.0f, 0.0f, 0.0f, 1.0f };

			glm::bvec4 m_RoundedCorners = { true, true, true, true }; // Top, left, botton, right.
			glm::bvec4 m_PreviousRoundedCorners;
			float m_RoundedRadius = 10.0f; // In pixels.
			ButtonAction m_ButtonAction = ButtonAction::RELEASED;
			glm::vec2 m_ListBoxPadding = { 5.0f, 5.0f };
			glm::vec2 m_ListButtonSize = { 200.0f, 35.0f };
			glm::vec2 m_CheckBoxSize = { 35.0f, 35.0f };
			glm::vec2 m_SliderRatioToRect = { 0.05f, 1.5f };
			glm::vec2 m_ButtonSize = { 200.0f, 35.0f};
			glm::vec2 m_ImageSize = { 64.0f, 64.0f };
			glm::vec2 m_SliderSize = { 200.0f, 35.0f };
			glm::vec2 m_BarSize = { 200.0f, 35.0f };
			int m_IsStatic = true;
			int m_CharsAfterDot = 3;
			float m_FontScale = 15;
			float m_ScrollBarWidthRelativeToRect = 0.1f; // From 0 to 1 of rect width.
			float m_ScrollScale = 5.0f;
			float m_SpaceBetweenLines = m_FontScale * 0.3f;
			bool m_ChangeColorWhenPanelIsHovered = false;
			float m_ScrollingByClickingOffset = 0.1f; // From 0 to 1 scaling of full height of current panel.
			size_t m_MaxItemInListBox = 8;
		};

		struct UIProperties {
		public:

			glm::vec2 m_Position;
			glm::vec2 m_Size;
			glm::vec2 m_Margin;
			glm::vec2 m_Padding;
			float m_YOffset;
			float m_PanelLabelSize = 24.0f;
			bool m_AutoAllignment = false;
		};

		struct TextProperties {
		public:

			float m_Width = 0.5f;
			float m_Edge = 0.1f;
			float m_BorderWidth = 0.0f;
			float m_BorderEdge = 0.01f;
			glm::vec4 m_OutLineColor = glm::vec4(1.0f);
			glm::vec2 m_ShadowOffset = glm::vec2(0.0f);
		};

		Theme m_Theme;

		glm::mat4 m_ViewProjection;

		// If @IsRelatedToPanel set to true then all of ui element's coordinates set to 0
		// will be created at the top left corner of the @panel.
		UIProperties m_RelatedPanelProperties;
		UIProperties m_LastUIPropertiesOnPanel;

		std::map <std::wstring, bool> m_Active;
		std::map <std::wstring, bool> m_NegativeInputs;
		std::map <std::wstring, std::string> m_DoubleInput;
		std::map <std::wstring, float> m_ScrollBars;

		std::wstring m_CurrentPressed;
		std::wstring m_CurrentDown;
		std::wstring m_CurrentHovered;

		bool m_IsInteracting = false;
		TextProperties m_TextProps;
		const Font* m_Font;

		enum AlignHorizontally {
			XCENTER = 1,
			LEFT = 2,
		};

		enum AlignVertically {
			YCENTER = 5,
			TOP = 3,
			BOTTOM = 4,
		};

		enum StandardFonts {
			CALIBRI = 0,
			ARIAL = 1,
			HARRINGTON = 2,
			PEAK = 3,
			PLAYBALL = 4,
			SEGOE = 5,
		};

		// Action that determines the behavior of any button.
		// It just determines when if statement is true.
		enum ButtonAction
		{
			DOWN,
			PRESSED,
			RELEASED
		};

		enum class Edge
		{
			TOPLEFT,
			BOTTOMLEFT,
			BOTTOMRIGHT,
			TOPRIGHT,
			TOP,
			LEFT,
			BOTTOM,
			RIGHT,
			ALL,
			NONE
		};

		AlignHorizontally m_XAlign = LEFT;
		AlignVertically m_YAlign = TOP;

		static Gui& GetInstance() { static Gui instance; return instance; }

		void BeginPanel(const std::wstring& label, glm::vec2 position, glm::vec2 size, Texture* texture = nullptr);
		void Panel(const std::wstring& label, glm::vec2 position, glm::vec2 size, Texture* texture = nullptr);
		void EndPanel();

		// If between @BeginPanel and @EndPanel @position should be set to zero
		// @position and 'size' in pixels from viewport -size * 0.5f to +size * 0.5f
		// @position is top-left corner.
		void Text(const std::wstring& text, glm::vec2 position, ...);
		void Image(glm::vec2 position, Texture* texture = nullptr, glm::vec2 size = { 0.0f, 0.0f });
		void Bar(glm::vec2 position, float value, float maxValue, glm::vec2 size = { 0.0f, 0.0f });
		void ListBox(const std::wstring& label, glm::vec2 position, std::vector<std::wstring> items, int& selectedItem, glm::vec2 size = { 0.0f, 0.0f });
		bool Button(const std::wstring& label, glm::vec2 position, Texture* texture = nullptr, glm::vec2 size = { 0.0f, 0.0f });
		bool CheckBox(const std::wstring& label, glm::vec2 position, bool* value, glm::vec2 size = { 0.0f, 0.0f });
		bool SliderFloat(const std::wstring& label, glm::vec2 position, glm::vec2 limits, float* value, glm::vec2 size = { 0.0f, 0.0f });
		bool CollapsingHeader(const std::wstring& label, glm::vec2 position, float height = 25.0f);

		void InputInt(const std::wstring& label, int64_t* value, float x = 0.0f, float y = 0.0f,
			float width = 100.0f, float height = 50.0f, glm::vec4 panelColor = glm::vec4(0.5, 0.5, 0.5, 1));

		void InputDouble(const std::wstring& label, double* value, float x = 0.0f, float y = 0.0f,
			float width = 100.0f, float height = 50.0f, glm::vec4 panelColor = glm::vec4(0.5, 0.5, 0.5, 1));

		bool IsAnyPanelHovered() const;
		bool IsRectHovered(const float vertices[8]);
		void Begin();
		void End();
		void SameLine();
		void SetFont(const Font& font);
		void RoundCorners(Edge edge);
		glm::vec2 GetFromWorldToScreenSpace(const glm::vec2 position);
		const Font& GetFont() const;
		const Font& GetStandardFont(StandardFonts standardFont) const;
		const float* GetCurrentPanelRect() const;
	private:
		
		std::vector<Font*> m_StandardFonts;

		Texture* m_CheckBoxTextureTrue = nullptr;
		Texture* m_CheckBoxTextureFalse = nullptr;
		Texture* m_TriangleRightTexture = nullptr;
		Texture* m_TriangleDownTexture = nullptr;

		float m_CurrentPanelCoords[8];
		UIProperties m_BackUpUIProperties;
		float m_BackUpCurrentPanelCoords[8];
		bool m_IsRelatedToPanel = false;
		bool m_IsAnyPanelHovered = false;
		bool m_IsRelatedToMainPanel = false;
		bool m_IsButtonClickable = true;
		bool m_IsCurrentPanelHovered = false;
		bool m_IsRelativeToHeight = true;
		float m_Aspect = 1.0f;
		float m_RelativeHeight = 1080.0f;
		glm::vec2 m_ViewportUISize;
		glm::vec2 m_CurrentPanelPosition;
		glm::vec2 m_CurrentPanelSize;
	private:

		// @position is a center of a rect, 'halfSize' is a length from the center to an edge.
		void CreateRect(float vertices[8], glm::vec2 position, glm::vec2 halfSize);
		bool ButtonBehavior(const std::wstring& uniqueId, float vertices[8], glm::vec4& color, bool isCurrentPanelHovered = true);
		float ScrollBar(const std::wstring& label, glm::vec2 position, glm::vec2 size, float actualHeight, float fullHeight, bool isHovered);
		void RecalculateProjectionMatrix();
		void ShutDown();
		void LoadStandartFonts();
		void ApplyRelatedToPanelProperties(float* x, float* y);
		Character* FindCharInFont(wchar_t character);

		Gui& operator=(const Gui& rhs) { return *this; }
		Gui(const Gui&) = delete;
		Gui();
		~Gui() {}

		friend class Batch;
		friend class Viewport;
	};

}