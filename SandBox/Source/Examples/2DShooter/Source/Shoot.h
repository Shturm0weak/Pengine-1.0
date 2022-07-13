#pragma once

#include "Core/Core.h"
#include "Core/Component.h"
#include "Core/Utils.h"
#include "EventSystem/Listener.h"
#include "Audio/Sound.h"

class Shoot : public Pengine::IComponent, public Pengine::IListener
{
	RTTR_ENABLE(IComponent)

private:

	void SpawnBloodEffect(const glm::vec3& position);
	void SpawnStonesEffect(const glm::vec3& position);
	void MuzzleFlash();
public:

	class Pawn* m_Pawn = nullptr;

	std::string m_TargetTag;

	public: PROPERTY(Shoot, int, m_Ammo, 30)
	public: PROPERTY(Shoot, int, m_MaxAmmo, 90)
	public: PROPERTY(Shoot, float, m_FireRate, 500.0f)
	public: PROPERTY(Shoot, float, m_CurrentAmmo, 90.0f)
	public: PROPERTY(Shoot, float, m_Damage, 5.0f)
	public: PROPERTY(Shoot, float, m_MuzzleFlashDuration, 0.03f)
public:
	
	float m_Timer = 0.0f;

	std::vector<std::string> m_IgnoreMask;

	static Pengine::GameObject* m_BloodPrefab;
	static Pengine::GameObject* m_StonesPrefab;

	class Pengine::Sound* m_FireSound = nullptr;

	virtual class Pengine::IComponent* New(class Pengine::GameObject* owner) override;
	virtual void Copy(const Pengine::IComponent& component) override;
	virtual void OnUpdate() override;

	Shoot();

	~Shoot();

	void Fire();

	static class Pengine::IComponent* Create(class Pengine::GameObject* owner);
};
REGISTER_CLASS(Shoot)