#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/ReflectionSystem.h"
#include "OpenGL/Texture.h"

#include <map>
#include <functional>

class Skills : public Pengine::IComponent
{
	RTTR_ENABLE(IComponent)
private:
	struct Skill
	{
		std::vector<std::wstring> m_NeededSkillToBeUnLocked;
		std::wstring m_Name;
		std::function<void()> m_UnLockCallback;
		float m_Price = 0.0f;
		float m_Effect = 0.0f;
		bool m_IsUnLocked = false;
	};

	std::map<std::wstring, Skill> m_Skills;

	class Pengine::Texture* m_ChosenSkill = nullptr;
	class Pengine::Texture* m_UnLockedSkill = nullptr;
	class Pengine::Texture* m_LockedSkill = nullptr;
	class Pengine::Texture* m_SkillsBackGround = nullptr;
	class Pengine::Texture* m_UnAvailable = nullptr;

	std::wstring m_ChosenSkillName;
public:

	class Player* m_Player = nullptr;

	Skills();
	~Skills() = default;

	virtual class Pengine::IComponent* New(class Pengine::GameObject* owner) override;
	virtual void Copy(const Pengine::IComponent& component) override;

	static class Pengine::IComponent* Create(class Pengine::GameObject* owner);

	Skill* GetSkill(const std::wstring& name);
	void UserInterface();
	void AddSkill(Skill skill);
	void UnLock(const std::wstring& name, float& currency);
	bool IsUnLocked(const std::wstring& name) const;
	bool SkillsNeededSatisfied(const std::wstring& name);
};
REGISTER_CLASS(Skills)