#include "2DShooter.h"

#include "Core/Serializer.h"
#include "UI/Gui.h"
#include "Audio/SoundManager.h"
#include "EventSystem/EventSystem.h"
#include "Events/OnMainThreadCallback.h"
#include "Core/Timer.h"
#include "Core/Utils.h"
#include "Core/Scene.h"
#include "Core/Input.h"
#include "Core/Viewport.h"

#include "Controller.h"
#include "Health.h"
#include "Shoot.h"
#include "EnemyAI.h"

using namespace Pengine;

GameObject* Shoot::m_BloodPrefab = nullptr;
GameObject* Shoot::m_StonesPrefab = nullptr;

void Shooter2D::SkillMenu()
{
	if (!m_SkillMenu.m_Activated) return;

	m_Player->GetOwner()->m_ComponentManager.GetComponent<Skills>()->UserInterface();
}

Shooter2D::Shooter2D(const std::string& title)
	: Application(title)
{
	
}

void Shooter2D::OnStart()
{
	Serializer::DeserializeScene("Source/Scenes/2DShooter.yaml");
	m_Player = GetScene()->FindGameObject("Player")->m_ComponentManager.AddComponent<Player>();

	std::vector<GameObject*> enemies = GetScene()->FindGameObjects("Enemy");
	for (GameObject* enemy : enemies)
	{
		Enemy* enemyComponent = enemy->m_ComponentManager.AddComponent<Enemy>();
		m_Enemies.push_back(enemyComponent);
	}

	Serializer::DeserializePrefab("Source/Prefabs/Blood.prefab");
	Shoot::m_BloodPrefab = GetScene()->FindGameObject("Blood");
	Shoot::m_BloodPrefab->m_IsEnabled = false;
	Shoot::m_BloodPrefab->m_IsSelectable = false;

	Serializer::DeserializePrefab("Source/Prefabs/Stones.prefab");
	Shoot::m_StonesPrefab = GetScene()->FindGameObject("Stones");
	Shoot::m_StonesPrefab->m_IsEnabled = false;
	Shoot::m_StonesPrefab->m_IsSelectable = false;

	Serializer::DeserializePrefab("Source/Prefabs/Ammo.prefab");
	m_AmmoPrefab = GetScene()->FindGameObject("Ammo");
	m_AmmoPrefab->m_IsEnabled = false;
	m_AmmoPrefab->m_IsSelectable = false;

	SoundManager::GetInstance().SetVolume(0.01f);
}

void Shooter2D::OnPostStart()
{
	auto death = [this](GameObject* gameObject) {
		GameObject* ammo = GetScene()->CreateGameObject();
		ammo->Copy(*m_AmmoPrefab);
		ammo->m_Transform.Translate(ammo->m_Transform.GetPosition() + gameObject->m_Transform.GetPosition());
		ammo->m_IsEnabled = true;
		ammo->m_IsSelectable = true;

		gameObject->m_ComponentManager.RemoveComponent(gameObject->m_ComponentManager.GetComponent<BoxCollider2D>());
		gameObject->m_ComponentManager.RemoveComponent(gameObject->m_ComponentManager.GetComponent<Rigidbody2D>());
		gameObject->m_ComponentManager.RemoveComponent(gameObject->m_ComponentManager.GetComponent<EnemyAI>());
		Animator2D* a2d = gameObject->m_ComponentManager.GetComponent<Animator2D>();
		a2d->SetCurrentAnimation(a2d->m_Animations[2]);
		a2d->Reset();
		a2d->m_EndCallbacks.push_back([=] {
			auto callback = [=]() {
				a2d->SetPlay(false);
				Timer::SetCallback([=] { gameObject->GetScene()->DeleteGameObject(gameObject); }, 1.0f);
			};
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		});
	};

	for (auto enemy : m_Enemies)
	{
		enemy->m_Health->SetOnDeathCallback([this, enemy, death] (Pawn* attacker) {
			auto enemyIter = std::find(m_Enemies.begin(), m_Enemies.end(), enemy);
			if (enemyIter != m_Enemies.end())
			{
				m_Enemies.erase(enemyIter);
			}

			attacker->m_Money += enemy->m_Price;
			death(enemy->GetOwner());
		});
	}

	m_Player->m_Health->SetOnDeathCallback([this, death] (Pawn* attacker) { death(m_Player->GetOwner()); });
}

void Shooter2D::OnUpdate()
{
	if (Input::KeyBoard::IsKeyPressed(Keycode::KEY_I))
	{
		m_SkillMenu.m_Activated = !m_SkillMenu.m_Activated;
	}
}

void Shooter2D::OnGuiRender()
{
	Gui& gui = Gui::GetInstance();
	gui.SetFont(gui.GetStandardFont(gui.ARIAL));
	gui.m_Theme.m_FontScale = 15.0f;
	gui.m_Theme.m_TextColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	gui.m_XAlign = Gui::AlignHorizontally::XCENTER;
	gui.m_Theme.m_CharsAfterDot = 0;
	for (auto enemy : m_Enemies)
	{
		if (enemy->m_Health)
		{
			gui.Bar(gui.GetFromWorldToScreenSpace(enemy->GetOwner()->m_Transform.GetPosition() + glm::vec3(0.0f, 0.5f, 0.0f)) - glm::vec2(37.5f, 0.0f), enemy->m_Health->GetCurrentHealth(), enemy->m_Health->GetMaxHealth(), { 75.0f, 20.0f });
			gui.Text(L"%f", gui.GetFromWorldToScreenSpace(enemy->GetOwner()->m_Transform.GetPosition() + glm::vec3(0.0f, 0.5f, 0.0f)), enemy->m_Health->GetCurrentHealth());
		}
	}

	gui.m_Theme.m_FontScale = 30.0f;
	GameObject* player = GetScene()->FindGameObject("Player");
	if (player && player->m_ComponentManager.GetComponent<Player>())
	{
		gui.Bar(-gui.m_ViewportUISize * 0.4f - glm::vec2(112.5f, 0.0f), m_Player->m_Health->GetCurrentHealth(), m_Player->m_Health->GetMaxHealth(), { 225.0f, 60.0f });
		gui.Text(L"%f", -gui.m_ViewportUISize * 0.4f - glm::vec2(0.0f, 15.0f), m_Player->m_Health->GetCurrentHealth());
		gui.Text(L"Ammo: %d", -gui.m_ViewportUISize * 0.4f - glm::vec2(0.0f, -40.0f), m_Player->m_Shoot->m_Ammo);
		gui.Text(L"Money: %f", -gui.m_ViewportUISize * 0.4f - glm::vec2(0.0f, -95.0f), m_Player->m_Money);
	}

	gui.m_Theme.m_FontScale = 15.0f;
	gui.m_XAlign = Gui::AlignHorizontally::LEFT;
	SkillMenu();
}

void Shooter2D::OnClose()
{
	m_Enemies.clear();
}

