#pragma once

#include "Core/Core.h"
#include "Core/Application.h"
#include "Core/TextureManager.h"
#include "Core/Viewport.h"
#include "UI/Gui.h"

#include <vector>

class UITest : public Pengine::Application
{
public:

	int m_SelectedItem = -1;
	std::vector<std::wstring> m_Items;
	Pengine::Texture* m_WolfTexture = nullptr;
	float m_Slider = 0.0f;
	bool m_Checkbox = false;

	UITest()
	{
		m_Items.reserve(100);
		for (size_t i = 0; i < 150; i++)
		{
			m_Items.push_back(L"Item " + std::to_wstring(i + 1));
		}
		Pengine::TextureManager::GetInstance().AsyncGet([=](Pengine::Texture* t) {
			m_WolfTexture = t;
			}, "Source/Images/Wolf.png");
	}

	virtual void OnGuiRender() override
	{
		Pengine::Gui& gui = Pengine::Gui::GetInstance();
		glm::vec2 mousePos = Pengine::Viewport::GetInstance().GetUIMousePosition();
		gui.SetFont(gui.GetStandardFont(gui.ARIAL));
		gui.m_Theme.m_FontScale = 30.0f;
		gui.m_Theme.m_ButtonSize = { 200.0f, 100.0f };
		gui.RoundCorners(Pengine::Gui::Edge::ALL);
		float tempFontScale = gui.m_Theme.m_FontScale;
		gui.m_Theme.m_FontScale *= 0.5f;
		gui.Text(L"Mouse pos X = %f; Y = %f", mousePos, mousePos.x, mousePos.y);
		gui.m_Theme.m_FontScale = tempFontScale;
		//gui.m_XAlign = gui.XCENTER;
		gui.BeginPanel(L"##TestPanel", { 0.0f, 0.0f }, { 1000.0f, 1000.0f });
		gui.Button(L"1", { 0.0f, 0.0f });
		gui.Button(L"2", { gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding });
		gui.Button(L"3", { (gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding) * 2.0f });
		gui.Button(L"4", { (gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding) * 3.0f });
		gui.m_RelatedPanelProperties.m_YOffset -= (gui.m_Theme.m_ButtonSize.y + gui.m_RelatedPanelProperties.m_Padding.y) * 3.0f;
		gui.SliderFloat(L"Slider", { 0.0f, 0.0f }, { 0.0f,30.0f }, &gui.m_Theme.m_RoundedRadius);
		gui.CheckBox(L"Checkbox", { 0.0f, 0.0 }, &m_Checkbox);
		if (gui.CollapsingHeader(L"Collapsing header", { 0.0f, 0.0f }, 50.0f))
		{
			gui.Text(L"Test\nmultilines\ninput", { 0.0f, 0.0f });
		}
		gui.Image({ 0.0f, 0.0f }, Pengine::TextureManager::GetInstance().Get("Wolf"));
		gui.ListBox(L"List", { 0.0f, 0.0f }, m_Items, m_SelectedItem);
		gui.EndPanel();
	}
};