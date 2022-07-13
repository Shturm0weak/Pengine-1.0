#pragma once

#include "Core/Application.h"

#include "Player.h"
#include "Enemy.h"

class Shooter2D : public Pengine::Application
{
private:
	
	Player* m_Player = nullptr;
	class Pengine::GameObject* m_AmmoPrefab = nullptr;

	std::vector<Enemy*> m_Enemies;

	struct SkillMenu
	{
		bool m_Activated = false;
	} m_SkillMenu;

	void SkillMenu();
public:

	Shooter2D(const std::string& title = "Shooter2D");

	virtual void OnStart() override;
	virtual void OnPostStart() override;
	virtual void OnUpdate() override;
	virtual void OnClose() override;
	virtual void OnGuiRender() override;
};