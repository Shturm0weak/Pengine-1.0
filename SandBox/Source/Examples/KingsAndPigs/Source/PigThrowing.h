#pragma once

#include "Core/Core.h"
#include "Core/Component.h"

#include "Pig.h"

using namespace Pengine;

class PigThrowing : public Pig
{
	REGISTER_PARENT_CLASS(Pig)

public: PROPERTY(std::string, m_ObjectName, "")
public: PROPERTY(glm::vec3, m_PigScale, { 0.8f COM 0.8f COM 0.8f })
public: PROPERTY(glm::vec3, m_ThrowingPigScale, { 0.6f COM 0.6f COM 0.6f })
private:

	GameObject* m_ThrowingObjectPrefab = nullptr;

	bool m_IsPicking = false;
protected:

	virtual void Attack() override;

	virtual void UpdateAnimation() override;
public:

	virtual IComponent* New(GameObject* owner) override;

	virtual void Copy(const IComponent& component) override;

	static IComponent* Create(GameObject* owner);

	virtual void OnStart() override;
};
REGISTER_COMPONENT(PigThrowing)