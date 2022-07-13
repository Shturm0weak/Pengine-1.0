#include "Skills.h"

#include "UI/Gui.h"
#include "Core/TextureManager.h"
#include "Core/GameObject.h"

#include "Health.h"
#include "Player.h"
#include "Shoot.h"

using namespace Pengine;

Skills::Skills()
{
	m_ChosenSkill = TextureManager::GetInstance().Create("Source/Examples/2DShooter/Sprites/ChosenSkill.png");
	m_SkillsBackGround = TextureManager::GetInstance().Create("Source/Examples/2DShooter/Sprites/SkillsBackGround.png");
	m_LockedSkill = TextureManager::GetInstance().Create("Source/Examples/2DShooter/Sprites/LockedSkill.png");
	m_UnLockedSkill = TextureManager::GetInstance().Create("Source/Examples/2DShooter/Sprites/UnLockedSkill.png");
	m_UnAvailable = TextureManager::GetInstance().Create("Source/Examples/2DShooter/Sprites/UnAvailable.png");

	float effect = 15.0f;
	float price = 100.0f;

	Skill health1 = { {}, L"+" + std::to_wstring((int)effect) + L" health", [] {}, price, effect, false };
	std::function<void()> callback = [this, effect]()
	{
		Health* health = this->m_Player->m_Health;
		health->SetMaxHealth(health->GetMaxHealth() + effect);
		health->SetCurrentHealth(health->GetCurrentHealth() + effect);
	};
	health1.m_UnLockCallback = callback;
	AddSkill(health1);

	Skill damage1 = { {}, L"+" + std::to_wstring((int)effect) + L" damage", [] {}, price, effect, false };
	callback = [this, effect]()
	{
		Shoot* shoot = this->m_Player->m_Shoot;
		shoot->m_Damage += effect;
	};
	damage1.m_UnLockCallback = callback;
	AddSkill(damage1);

	Skill fireRate1 = { {}, L"+" + std::to_wstring((int)effect) + L" firerate", [] {}, price, effect, false };
	callback = [this, effect]()
	{
		Shoot* shoot = this->m_Player->m_Shoot;
		shoot->m_FireRate += effect;
	};
	fireRate1.m_UnLockCallback = callback;
	AddSkill(fireRate1);

	Skill armor1 = { {}, L"+" + std::to_wstring((int)effect) + L" armor", [] {}, price, effect, false };
	callback = [this, effect]()
	{

	};
	armor1.m_UnLockCallback = callback;
	AddSkill(armor1);



	effect *= 2.0f;
	price *= 2.0f;

	Skill health2 = { { health1.m_Name }, L"+" + std::to_wstring((int)effect) + L" health", [] {}, price, effect, false };
	callback = [this, effect]()
	{
		Health* health = this->m_Player->m_Health;
		health->SetMaxHealth(health->GetMaxHealth() + effect);
		health->SetCurrentHealth(health->GetCurrentHealth() + effect);
	};
	health2.m_UnLockCallback = callback;
	AddSkill(health2);

	Skill damage2 = { { damage1.m_Name }, L"+" + std::to_wstring((int)effect) + L" damage", [] {}, price, effect, false };
	callback = [this, effect]()
	{
		Shoot* shoot = this->m_Player->m_Shoot;
		shoot->m_Damage += effect;
	};
	damage2.m_UnLockCallback = callback;
	AddSkill(damage2);

	Skill fireRate2 = { { fireRate1.m_Name }, L"+" + std::to_wstring((int)effect) + L" firerate", [] {}, price, effect, false };
	callback = [this, effect]()
	{
		Shoot* shoot = this->m_Player->m_Shoot;
		shoot->m_FireRate += effect;
	};
	fireRate2.m_UnLockCallback = callback;
	AddSkill(fireRate2);

	Skill armor2 = { { armor1.m_Name }, L"+" + std::to_wstring((int)effect) + L" armor", [] {}, price, effect, false };
	callback = [this, effect]()
	{

	};
	armor2.m_UnLockCallback = callback;
	AddSkill(armor2);
}

IComponent* Skills::New(GameObject* owner)
{
	return Create(owner);
}

void Skills::Copy(const IComponent& component)
{
	Skills& skills = *(Skills*)&component;
	m_Type = component.GetType();
	m_Skills = skills.m_Skills;
}

IComponent* Skills::Create(GameObject* owner)
{
	return new Skills();
}

Skills::Skill* Skills::GetSkill(const std::wstring& name)
{
	auto skillIter = m_Skills.find(name);
	if (skillIter != m_Skills.end())
	{
		return &(skillIter->second);
	}

	return nullptr;
}

void Skills::UserInterface()
{
	Gui& gui = Gui::GetInstance();
	gui.SetFont(gui.GetStandardFont(gui.PEAK));
	gui.m_Theme.m_FontScale = 10.0f;
	gui.m_Theme.m_ButtonSize = { 200.0f, 100.0f };
	gui.m_Theme.m_PanelColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	gui.RoundCorners(Gui::Edge::ALL);
	gui.BeginPanel(L"##SkillsPanel", { 0.0f, 0.0f }, { 1000.0f, 1000.0f }, m_SkillsBackGround);

	glm::vec2 tempPadding = gui.m_RelatedPanelProperties.m_Padding;
	gui.m_RelatedPanelProperties.m_Margin = { 35.0f, 35.0f };
	gui.m_RelatedPanelProperties.m_Padding = { 43.3f, 40.0f };

	gui.m_Theme.m_TextColor = { 0.75f, 0.75f, 0.75f, 1.0f };

	gui.m_XAlign = Gui::AlignHorizontally::LEFT;
	gui.m_Theme.m_FontScale *= 2.0f;
	gui.Text(L"Skills", { 380.0f, 0.0f });
	gui.m_Theme.m_FontScale *= 0.5f;
	gui.m_XAlign = Gui::AlignHorizontally::LEFT;

	glm::vec4 buttonColor = gui.m_Theme.m_ButtonColor;

	auto getBackGround = [this](const std::wstring& name)
	{
		if (!SkillsNeededSatisfied(name))
		{
			return m_UnAvailable;
		}
		else if (IsUnLocked(name))
		{
			return m_UnLockedSkill;
		}
		else if (m_ChosenSkillName == name)
		{
			return m_ChosenSkill;
		}
		else
		{
			return m_LockedSkill;
		}
	};

	size_t counter = 0;
	for (auto skill : m_Skills)
	{
		Texture* backGround = getBackGround(skill.second.m_Name);

		if (gui.Button(skill.second.m_Name, { (gui.m_Theme.m_ButtonSize + gui.m_RelatedPanelProperties.m_Padding) * (float)counter }, backGround))
		{
			if (backGround != m_UnLockedSkill)
			{
				m_ChosenSkillName = skill.second.m_Name;
			}
		}

		if ((counter + 1) % 4 == 0)
		{
			gui.m_RelatedPanelProperties.m_YOffset -= (gui.m_Theme.m_ButtonSize.y + gui.m_RelatedPanelProperties.m_Padding.y) * counter;
			counter = 0;
		}
		else
		{
			counter++;
		}
	}

	gui.m_Theme.m_FontScale *= 2.0f;
	gui.m_XAlign = Gui::AlignHorizontally::XCENTER;
	gui.Text(L"Price: %f", { 480.0f, 0.0f }, m_ChosenSkillName.empty() ? 0 : GetSkill(m_ChosenSkillName)->m_Price);

	gui.m_Theme.m_ButtonColor = m_ChosenSkillName.empty() ? glm::vec4(glm::vec3(buttonColor * 0.5f), 1.0f) : buttonColor;

	if (gui.Button(L"Buy", { 0.0f, 0.0f }, m_LockedSkill))
	{
		UnLock(m_ChosenSkillName, m_Player->m_Money);
	}

	gui.m_Theme.m_ButtonColor = buttonColor;

	gui.m_RelatedPanelProperties.m_Padding = tempPadding;
	gui.EndPanel();
}

void Skills::AddSkill(Skill skill)
{
	m_Skills.insert(std::make_pair(skill.m_Name, skill));
}

void Skills::UnLock(const std::wstring& name, float& currency)
{
	auto skillIter = m_Skills.find(name);
	if (skillIter != m_Skills.end())
	{
		if (currency < skillIter->second.m_Price) return;

		if (!SkillsNeededSatisfied(name)) return;

		currency -= skillIter->second.m_Price;
		skillIter->second.m_IsUnLocked = true;

		if (skillIter->second.m_UnLockCallback)
		{
			skillIter->second.m_UnLockCallback();
		}
	}
}

bool Skills::IsUnLocked(const std::wstring& name) const
{
	for (auto skill : m_Skills)
	{
		if (skill.second.m_Name == name)
		{
			if (skill.second.m_IsUnLocked)
			{
				return true;
			}
		}
	}

	return false;
}

bool Skills::SkillsNeededSatisfied(const std::wstring& name)
{
	for (auto skill : GetSkill(name)->m_NeededSkillToBeUnLocked)
	{
		if (!IsUnLocked(skill))
		{
			return false;
		}
	}

	return true;
}
